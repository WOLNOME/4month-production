#pragma once
#include <d2d1_3.h>
#include <wrl.h>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class D2DRender {
private://コンストラクタ等の隠蔽
	static D2DRender* instance;
	D2DRender() = default;//コンストラクタ隠蔽
	~D2DRender() = default;//デストラクタ隠蔽
	D2DRender(D2DRender&) = delete;//コピーコンストラクタ封印
	D2DRender& operator=(D2DRender&) = delete;//コピー代入演算子封印
public:
	//シングルトンインスタンスの取得
	static D2DRender* GetInstance();
public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//終了
	void Finalize();
	//描画
	void Draw();
public://getter
	//D2DDeviceContext
	ID2D1DeviceContext2* GetD2DDeviceContext() const { return d2dDeviceContext.Get(); }
	//D2DFactory
	ID2D1Factory3* GetD2DFactory() const { return d2dFactory.Get(); }
private:
	//D2DDeviceContextの生成
	void CreateD2DDeviceContext();
	//D2DFactoryの生成
	void CreateD2DFactory();

private:
	//D2DDevice
	ComPtr<ID2D1DeviceContext2> d2dDeviceContext = nullptr;
	//D2DFactory
	ComPtr<ID2D1Factory3> d2dFactory = nullptr;
};

