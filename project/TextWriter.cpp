#include "TextWriter.h"
#include <cassert>

void TextWriter::Initialize() {
	//DWriteFactoryの生成
	CreateIDWriteFactory();
	//D3D11On12Deviceの生成
	CreateD3D11On12Device();
	//D2DDeviceContextの生成
	CreateDirect2DDeviceContext();
	//D2DRenderTargetの生成
	CreateD2DRenderTarget();
}

void TextWriter::Update() {
}

void TextWriter::Draw() {
}

void TextWriter::CreateIDWriteFactory() {
	HRESULT hr;
	//IDWriteFactoryの生成
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &directWriteFactory);
	assert(SUCCEEDED(hr));
}

void TextWriter::CreateD3D11On12Device() {
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
		reinterpret_cast<IUnknown**>(dxcommon->GetAddlessOfCommandQueue()),
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

void TextWriter::CreateDirect2DDeviceContext() {
	HRESULT hr;
	//ID2D1Factory3の生成
	ComPtr<ID2D1Factory3> d2dFactory = nullptr;
	constexpr D2D1_FACTORY_OPTIONS factoryOptions{};
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &factoryOptions, &d2dFactory);
	assert(SUCCEEDED(hr));
	//IDXGIDeviceの生成
	ComPtr<IDXGIDevice> dxgiDevice = nullptr;
	hr = d3d11On12Device.As(&dxgiDevice);
	assert(SUCCEEDED(hr));
	//ID2D1Device2の生成
	ComPtr<ID2D1Device2> d2dDevice = nullptr;
	hr = d2dFactory->CreateDevice(dxgiDevice.Get(), d2dDevice.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));
	//d2dDeviceContextの生成
	hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, d2dDeviceContext.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));
}

void TextWriter::CreateD2DRenderTarget() {
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
