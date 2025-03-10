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
#include <memory>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class TextWrite;

class TextWriteManager {
private://コンストラクタ等の隠蔽
	static TextWriteManager* instance;

	TextWriteManager() = default;//コンストラクタ隠蔽
	~TextWriteManager() = default;//デストラクタ隠蔽
	TextWriteManager(TextWriteManager&) = delete;//コピーコンストラクタ封印
	TextWriteManager& operator=(TextWriteManager&) = delete;//コピー代入演算子封印
public:
	//シングルトンインスタンスの取得
	static TextWriteManager* GetInstance();
public:

	void Initialize();
	void Finalize();

	//個別クラスを登録
	void Registration(TextWrite* piece);
	//個別クラスの登録を解除
	void CancelRegistration(const std::string& key);

private:
	///=======================
	/// 初期化時処理
	///=======================

	void CreateIDWriteFactory();
	void CreateD3D11On12Device();
	void CreateDirect2DDeviceContext();
	void CreateD2DRenderTarget();

public:
	///=======================
	/// 描画前準備
	///=======================

	void EditSolidColorBrash(const std::string& key, const D2D1::ColorF color) noexcept;
	void EditTextFormat(const std::string& key, const std::wstring& fontName, const float fontSize) noexcept;

	///=======================
	/// 描画処理
	///=======================

	void BeginDrawWithD2D()const noexcept;
	void WriteText(const std::string& key) const noexcept;
	void EndDrawWithD2D() const noexcept;




private:
	//省略変数
	WinApp* winapp = WinApp::GetInstance();
	DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	MainRender* mainrender = MainRender::GetInstance();

	//保存用変数
	ComPtr<IDWriteFactory> directWriteFactory = nullptr;
	ComPtr<ID3D11On12Device> d3d11On12Device = nullptr;
	ComPtr<ID3D11DeviceContext> d3d11On12DeviceContext = nullptr;
	ComPtr<ID2D1DeviceContext2> d2dDeviceContext = nullptr;

	std::vector<ComPtr<ID3D11Resource>> wrappedBackBuffers;
	std::vector<ComPtr<ID2D1Bitmap1>> d2dRenderTargets;

	std::unordered_map<std::string, ComPtr<ID2D1SolidColorBrush>> solidColorBrushMap;
	std::unordered_map<std::string, ComPtr<IDWriteTextFormat>> textFormatMap;

	//テキストライトコンテナ
	std::unordered_map<std::string, TextWrite*> textWriteMap;

};
