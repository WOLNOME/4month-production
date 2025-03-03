#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "MainRender.h"
#include <d2d1_3.h>
#include <d3d11on12.h>
#include <dwrite_3.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <unordered_map>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class TextWriter {
public:

	void Initialize();
	void Update();
	void Draw();

	///=======================
	/// 初期化時処理
	///=======================

	bool CreateIDWriteFactory() noexcept;
	bool CreateD3D11On12Device() noexcept;
	bool CreateDirect2DDeviceContext() noexcept;
	bool CreateD2DRenderTarget() noexcept;



	///=======================
	/// 描画前準備
	///=======================

	void registerSolidColorBrash(const std::string& key, const D2D1::ColorF color) noexcept;
	void registerTextFormat(const std::string& key, const std::wstring& fontName, const float fontSize) noexcept;

	///=======================
	/// 描画処理
	///=======================
	
	void BeginDrawWithD2D()const noexcept;



private:
	//省略変数
	WinApp* winapp = WinApp::GetInstance();
	DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	MainRender* mainrender = MainRender::GetInstance();

	//保存用変数
	ComPtr<IDWriteFactory> directWriteFactory;
	ComPtr<ID3D11On12Device> d3d11On12Device = nullptr;
	ComPtr<ID3D11DeviceContext> m_d3d11DeviceContext = nullptr;
	ComPtr<ID2D1DeviceContext2> d2dDeviceContext = nullptr;

	std::vector<ComPtr<ID3D11Resource>> wrappedBackBuffers;
	std::vector<ComPtr<ID2D1Bitmap1>> d2dRenderTargets;

	std::unordered_map<std::string, ComPtr<ID2D1SolidColorBrush>> solidColorBrushMap;
	std::unordered_map<std::string, ComPtr<IDWriteTextFormat>> textFormatMap;
};
