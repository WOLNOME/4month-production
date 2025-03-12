#include "TextWriteManager.h"
#include "TextWrite.h"
#include <filesystem>
#include <cassert>
namespace fs = std::filesystem;

TextWriteManager* TextWriteManager::instance = nullptr;

TextWriteManager* TextWriteManager::GetInstance() {
	if (instance == nullptr) {
		instance = new TextWriteManager;
	}
	return instance;
}

void TextWriteManager::Initialize() {
	//DWriteFactoryの生成
	CreateIDWriteFactory();
	//D3D11On12Deviceの生成
	CreateD3D11On12Device();
	//D2DDeviceContextの生成
	CreateDirect2DDeviceContext();
	//D2DRenderTargetの生成
	CreateD2DRenderTarget();

	//フォントファイルの生成
	CreateFontFile();
}

void TextWriteManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void TextWriteManager::Registration(TextWrite* piece) {
	std::string key = piece->GetName();

	//コンテナに同じ名前が登録されていたらエラー
	if (textWriteMap.find(key) != textWriteMap.end()) {
		assert(0 && "TextWriteで既に同じ名前が使われています。");
	}

	//コンテナに登録
	textWriteMap[key] = piece;

	//ブラシを登録
	EditSolidColorBrash(key, textWriteMap[key]->GetColor());
	//フォントを登録
	EditTextFormat(key, textWriteMap[key]->GetFontName(), textWriteMap[key]->GetSize());
}

void TextWriteManager::CancelRegistration(const std::string& key) {
	// 指定したキーが存在するか確認
	auto it = textWriteMap.find(key);
	if (it != textWriteMap.end()) {
		// キーと関連する情報を削除
		textWriteMap.erase(it);
	}
	else {
		// キーが見つからなかった場合の処理
		return;
	}

	//ブラシとフォントのコンテナの情報も削除
	if (solidColorBrushMap.find(key) != solidColorBrushMap.end()) {
		// キーと関連する情報を削除
		solidColorBrushMap.erase(solidColorBrushMap.find(key));
	}
	else {
		// キーが見つからなかった場合の処理
		assert(0 && "発生するはずのないエラーです。システムに問題がある可能性が高いです。");
		return;
	}
	if (textFormatMap.find(key) != textFormatMap.end()) {
		// キーと関連する情報を削除
		textFormatMap.erase(textFormatMap.find(key));
	}
	else {
		// キーが見つからなかった場合の処理
		assert(0 && "発生するはずのないエラーです。システムに問題がある可能性が高いです。");
		return;
	}

}

void TextWriteManager::CreateIDWriteFactory() {
	HRESULT hr;
	//IDWriteFactoryの生成
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &directWriteFactory);
	assert(SUCCEEDED(hr));
}

void TextWriteManager::CreateD3D11On12Device() {
	HRESULT hr;
	//ID3D11On12Deviceの生成
	ComPtr<ID3D11Device
	> d3d11Device = nullptr;
	UINT d3d11DeviceFlags = 0U;
#ifdef _DEBUG
	d3d11DeviceFlags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#else
	d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif // _DEBUG
	hr = D3D11On12CreateDevice(
		dxcommon->GetDevice(),
		d3d11DeviceFlags,
		nullptr,
		0,
		reinterpret_cast<IUnknown**>(dxcommon->GetAddressOfCommandQueue()),
		1,
		0,
		&d3d11Device,
		&d3d11On12DeviceContext,
		nullptr
	);
	assert(SUCCEEDED(hr));
	//D3D11->D3D11On12
	hr = d3d11Device.As(&d3d11On12Device);
	assert(SUCCEEDED(hr));
}

void TextWriteManager::CreateDirect2DDeviceContext() {
	HRESULT hr;
	//ID2D1Factory3の生成
	D2D1_FACTORY_OPTIONS factoryOptions{};
	factoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &factoryOptions, &d2dFactory);
	assert(SUCCEEDED(hr));
	//IDXGIDeviceの生成
	ComPtr<IDXGIDevice> dxgiDevice = nullptr;
	hr = d3d11On12Device.As(&dxgiDevice);
	assert(SUCCEEDED(hr));
	//ID2D1Device2の生成
	ComPtr<ID2D1Device2> d2dDevice = nullptr;
	hr = d2dFactory->CreateDevice(dxgiDevice.Get(), d2dDevice.ReleaseAndGetAddressOf());

	wchar_t message[256];
	swprintf_s(message, L"HRESULT=0x%08X\n", hr);
	OutputDebugString(message);
	assert(SUCCEEDED(hr));
	//d2dDeviceContextの生成
	hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, d2dDeviceContext.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));
}

void TextWriteManager::CreateD2DRenderTarget() {
	HRESULT hr;
	//DirectWriteの描画先の生成
	D3D11_RESOURCE_FLAGS resourceFlags = { D3D11_BIND_RENDER_TARGET };
	const UINT dpi = GetDpiForWindow(winapp->GetHwnd());
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), static_cast<float>(dpi), static_cast<float>(dpi));

	for (UINT i = 0U; i < mainrender->GetBackBufferCount(); ++i) {
		ComPtr<ID3D11Resource> wrappedBackBuffer = nullptr;
		//ID3D11Resourceの生成
		hr = d3d11On12Device->CreateWrappedResource(mainrender->GetSwapChainResource(i), &resourceFlags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(wrappedBackBuffer.ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(hr));
		//IDXGISurfaceの生成
		ComPtr<IDXGISurface> dxgiSurface = nullptr;
		hr = wrappedBackBuffer.As(&dxgiSurface);
		assert(SUCCEEDED(hr));
		//ID2D1Bitmap1の生成
		ComPtr<ID2D1Bitmap1> d2dRenderTarget = nullptr;
		hr = d2dDeviceContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &bitmapProperties, &d2dRenderTarget);
		assert(SUCCEEDED(hr));

		wrappedBackBuffers.emplace_back(wrappedBackBuffer);
		d2dRenderTargets.emplace_back(d2dRenderTarget);
	}
}

void TextWriteManager::CreateFontFile() {
	HRESULT hr;
	// IDWriteFontSetBuilder2 の生成
	ComPtr<IDWriteFontSetBuilder2> dwriteFontSetBuilder = nullptr;
	hr = directWriteFactory->CreateFontSetBuilder(&dwriteFontSetBuilder);
	assert(SUCCEEDED(hr));

	std::vector<ComPtr<IDWriteFontFile>> fontFiles;
	std::wstring fontDirectory = L"Resources/fonts"; // フォントフォルダのパス

	// fontsフォルダ内の .ttf 及び .ttc ファイルを探索
	for (const auto& entry : fs::directory_iterator(fontDirectory)) {
		if (entry.is_regular_file() && (entry.path().extension() == L".ttf" || entry.path().extension() == L".ttc")) {
			// IDWriteFontFile の生成
			ComPtr<IDWriteFontFile> dwriteFontFile;
			hr = directWriteFactory->CreateFontFileReference(entry.path().c_str(), nullptr, &dwriteFontFile);
			if (SUCCEEDED(hr)) {
				fontFiles.push_back(dwriteFontFile); // 一時的な vector に保存
				hr = dwriteFontSetBuilder->AddFontFile(dwriteFontFile.Get()); // フォントセットビルダーに追加
				assert(SUCCEEDED(hr));
			}
		}
	}

	// IDWriteFontSet の生成
	ComPtr<IDWriteFontSet> dwriteFontSet = nullptr;
	hr = dwriteFontSetBuilder->CreateFontSet(&dwriteFontSet);
	assert(SUCCEEDED(hr));

	// フォントコレクションの生成
	hr = directWriteFactory->CreateFontCollectionFromFontSet(dwriteFontSet.Get(), &dwriteFontCollection);
	assert(SUCCEEDED(hr));
}

void TextWriteManager::EditSolidColorBrash(const std::string& key, const Vector4& color) noexcept {
	//色と透明度を分離
	D2D1::ColorF rgb(color.x, color.y, color.z);
	FLOAT alpha = static_cast<FLOAT>(color.w);
	//ブラシを作って登録(すでに作っていたら編集)
	ComPtr<ID2D1SolidColorBrush> brush = nullptr;
	d2dDeviceContext->CreateSolidColorBrush(rgb, &brush);
	brush->SetOpacity(alpha);
	solidColorBrushMap[key] = brush;
}

void TextWriteManager::EditTextFormat(const std::string& key, const std::wstring& fontName, const float fontSize) noexcept {
	//テキストフォーマットを作って登録(すでに作ってあったら編集)
	ComPtr<IDWriteTextFormat> textFormat = nullptr;
	directWriteFactory->CreateTextFormat(fontName.c_str(), dwriteFontCollection.Get(), DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"ja-jp", &textFormat);
	textFormatMap[key] = textFormat;
}

void TextWriteManager::BeginDrawWithD2D() const noexcept {
	const auto backBufferIndex = mainrender->GetSwapChain()->GetCurrentBackBufferIndex();
	const auto wrappedBackBuffer = wrappedBackBuffers[backBufferIndex];
	const auto backBufferForD2D = d2dRenderTargets[backBufferIndex];

	d3d11On12Device->AcquireWrappedResources(wrappedBackBuffer.GetAddressOf(), 1);
	d2dDeviceContext->SetTarget(backBufferForD2D.Get());
	d2dDeviceContext->BeginDraw();
	d2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
}

void TextWriteManager::WriteText(const std::string& key) {
	const auto textFormat = textFormatMap.at(key);
	const auto solidColorBrush = solidColorBrushMap.at(key);
	const auto textWrite = textWriteMap.at(key);

	//描画範囲
	D2D1_RECT_F rect;
	rect = {
		textWrite->GetPosition().x,
		textWrite->GetPosition().y,
		textWrite->GetPosition().x + textWrite->GetWidth(),
		textWrite->GetPosition().y + textWrite->GetHeight()
	};
	//テキスト描画処理
	d2dDeviceContext->DrawTextW(textWrite->GetText().c_str(), static_cast<UINT32>(textWrite->GetText().length()), textFormat.Get(), &rect, solidColorBrush.Get());

}

void TextWriteManager::EndDrawWithD2D() const noexcept {
	const auto backBufferIndex = mainrender->GetSwapChain()->GetCurrentBackBufferIndex();
	const auto wrappedBackBuffer = wrappedBackBuffers[backBufferIndex];

	d2dDeviceContext->EndDraw();
	d3d11On12Device->ReleaseWrappedResources(wrappedBackBuffer.GetAddressOf(), 1);
	//描画内容の確定(スワップ可能状態に移行)
	d3d11On12DeviceContext->Flush();
}
