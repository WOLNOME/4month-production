#include "TextWriter.h"
#include <cassert>

void TextWriter::Initialize() {
	//IDWriteFactoryの生成
	if (!CreateIDWriteFactory()) {
		assert(0 && "IDWriteFactoryの生成に失敗しました");
	}
	//D3D11On12Deviceの生成
	if (!CreateD3D11On12Device()) {
		assert(0 && "D3D11On12Deviceの生成に失敗しました");
	}
	//Direct2DDeviceContextの生成
	if (!CreateDirect2DDeviceContext()) {
		assert(0 && "Direct2DDeviceContextの生成に失敗しました");
	}
	//D2DRenderTargetの生成
	if (!CreateD2DRenderTarget()) {
		assert(0 && "D2DRenderTargetの生成に失敗しました");
	}
}

void TextWriter::Update() {
}

void TextWriter::Draw() {
	
}

bool TextWriter::CreateIDWriteFactory() noexcept {
	//IDWriteFactoryの生成
	if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &directWriteFactory))) [[unlikely]] {
		assert(0 && "IDWriteFactoryの生成に失敗しました");
		return false;
	}
	return true;
}

bool TextWriter::CreateD3D11On12Device() noexcept {
	//ID3D11On12Deviceの生成
	ComPtr<ID3D11Device
	> d3d11Device = nullptr;
	UINT d3d11DeviceFlags = 0U;

#ifdef _DEBUG
	d3d11DeviceFlags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#else
	d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif // _DEBUG

	if (FAILED(D3D11On12CreateDevice(
		dxcommon->GetDevice(),
		d3d11DeviceFlags,
		nullptr,
		0,
		reinterpret_cast<IUnknown**>(dxcommon->GetAddlessOfCommandQueue()),
		1,
		0,
		&d3d11Device,
		&d3d11On12DeviceContext,
		nullptr
	))) [[unlikely]] {
		assert(0 && "ID3D11On12Deviceの生成に失敗しました");
		return false;
	}
	//D3D11->D3D11On12
	return SUCCEEDED(d3d11Device.As(&d3d11On12Device));
}

bool TextWriter::CreateDirect2DDeviceContext() noexcept {
	//ID2D1Factory3の生成
	ComPtr<ID2D1Factory3> d2dFactory = nullptr;
	constexpr D2D1_FACTORY_OPTIONS factoryOptions{};
	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &factoryOptions, &d2dFactory))) [[unlikely]] {
		assert(0 && "ID2D1Factory3の生成に失敗しました");
		return false;
	}
	//IDXGIDeviceの生成
	ComPtr<IDXGIDevice> dxgiDevice = nullptr;
	if (FAILED(d3d11On12Device.As(&dxgiDevice))) [[unlikely]] {
		assert(0 && "IDXGIDeviceの生成に失敗しました");
		return false;
	}
	//ID2D1Device2の生成
	ComPtr<ID2D1Device2> d2dDevice = nullptr;
	if (FAILED(d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice))) [[unlikely]] {
		assert(0 && "ID2D1Device2の生成に失敗しました");
		return false;
	}
	
	return SUCCEEDED(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, d2dDeviceContext.ReleaseAndGetAddressOf()));
}

bool TextWriter::CreateD2DRenderTarget() noexcept {
	//DirectWriteの描画先の生成
	D3D11_RESOURCE_FLAGS resourceFlags = { D3D11_BIND_RENDER_TARGET };
	const UINT dpi = GetDpiForWindow(winapp->GetHwnd());
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), static_cast<float>(dpi), static_cast<float>(dpi));

	for (UINT i = 0U; i < mainrender->GetBackBufferCount(); ++i) {
		ComPtr<ID3D11Resource> wrappedBackBuffer = nullptr;
		//ID3D11Resourceの生成
		if (FAILED(d3d11On12Device->CreateWrappedResource(mainrender->GetSwapChainResource(i), &resourceFlags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(wrappedBackBuffer.ReleaseAndGetAddressOf())))) [[unlikely]] {
			assert(0 && "ID3D11On12Device::CreateWrappedResourceの呼び出しに失敗しました");
			return false;
		}
		//IDXGISurfaceの生成
		ComPtr<IDXGISurface> dxgiSurface = nullptr;
		if (FAILED(wrappedBackBuffer.As(&dxgiSurface))) [[unlikely]] {
			assert(0 && "IDXGISurfaceの生成に失敗しました");
			return false;
		}
		//ID2D1Bitmap1の生成
		ComPtr<ID2D1Bitmap1> d2dRenderTarget = nullptr;
		if (FAILED(d2dDeviceContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &bitmapProperties, &d2dRenderTarget))) [[unlikely]] {
			assert(0 && "ID2D1Bitmap1の生成に失敗しました");
			return false;
		}
		wrappedBackBuffers.emplace_back(wrappedBackBuffer);
		d2dRenderTargets.emplace_back(d2dRenderTarget);
	}
	return true;
}

void TextWriter::registerSolidColorBrash(const std::string& key, const D2D1::ColorF color) noexcept {
	//ブラシが登録されているか確認
	if (solidColorBrushMap.contains(key)) [[unlikely]] {
		return;
	}

	ComPtr<ID2D1SolidColorBrush> brush = nullptr;
	d2dDeviceContext->CreateSolidColorBrush(color, &brush);
	solidColorBrushMap[key] = brush;
}

void TextWriter::registerTextFormat(const std::string& key, const std::wstring& fontName, const float fontSize) noexcept {
	ComPtr<IDWriteTextFormat> textFormat = nullptr;
	directWriteFactory->CreateTextFormat(fontName.c_str(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"ja-jp", &textFormat);
	textFormatMap[key] = textFormat;
}

void TextWriter::BeginDrawWithD2D() const noexcept {
	const auto backBufferIndex = mainrender->GetSwapChain()->GetCurrentBackBufferIndex();
	const auto wrappedBackBuffer = wrappedBackBuffers[backBufferIndex];
	const auto backBufferForD2D = d2dRenderTargets[backBufferIndex];

	d3d11On12Device->AcquireWrappedResources(wrappedBackBuffer.GetAddressOf(), 1);
	d2dDeviceContext->SetTarget(backBufferForD2D.Get());
	d2dDeviceContext->BeginDraw();
	d2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
}

void TextWriter::WriteText(const std::string& textFormatKey, const std::string& solidColorBrushKey, const std::wstring& text, const D2D1_RECT_F& rect) const noexcept {
	const auto textFormat = textFormatMap.at(textFormatKey);
	const auto solidColorBrush = solidColorBrushMap.at(solidColorBrushKey);
	//描画処理
	d2dDeviceContext->DrawTextW(text.c_str(), static_cast<UINT32>(text.length()), textFormat.Get(), &rect, solidColorBrush.Get());
}

void TextWriter::EndDrawWithD2D() const noexcept {
	const auto backBufferIndex = mainrender->GetSwapChain()->GetCurrentBackBufferIndex();
	const auto wrappedBackBuffer = wrappedBackBuffers[backBufferIndex];

	d2dDeviceContext->EndDraw();
	d3d11On12Device->ReleaseWrappedResources(wrappedBackBuffer.GetAddressOf(), 1);
	//描画内容の確定(スワップ可能状態に移行)
	d3d11On12DeviceContext->Flush();
}
