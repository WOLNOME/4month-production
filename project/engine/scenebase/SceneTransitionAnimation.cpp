#include "SceneTransitionAnimation.h"
#include "WinApp.h"
#include <cassert>

SceneTransitionAnimation::SceneTransitionAnimation()
	: state_(TransitionState::NONE), type_(TransitionType::NONE), frame_(0), timer_(0), isTransitioning_(false), alpha_(1.0f) {
}

SceneTransitionAnimation::~SceneTransitionAnimation() {
}

void SceneTransitionAnimation::Initialize() {
	//Direct2Dの初期化
	InitD2D1();

	//変数の初期化
	state_ = TransitionState::NONE;
	type_ = TransitionType::NONE;
	frame_ = 0;
	timer_ = 0;
	isTransitioning_ = false;
}

void SceneTransitionAnimation::Update() {
	
}

void SceneTransitionAnimation::Draw() {
	//描画
	if (isTransitioning_) {
		DrawD2D1();
	}
}

void SceneTransitionAnimation::StartTransition() {
	//必要な変数の確認
	if (type_ == TransitionType::NONE) {
		assert(0 && "遷移の種類が設定されていません");
	}
	if (frame_ == 0) {
		assert(0 && "フレームが設定されていません");
	}

	//フェードイン開始
	state_ = TransitionState::UPDATE_IN;
	//遷移中フラグを立てる
	isTransitioning_ = true;
}

void SceneTransitionAnimation::UpdateIn() {
	switch (type_) {
	case SceneTransitionAnimation::TransitionType::FADE:
		//フェードイン処理
		if (state_ == TransitionState::UPDATE_IN) {
			//フレームが0になったら
			if (timer_ == 0) {
				//フェードイン終了
				state_ = TransitionState::END_IN;
			}
			else {
				//フレームを減らす
				timer_--;
				//透明度を計算
				float alpha = 1.0f - MyMath::Lerp(0.0f, 1.0f, static_cast<float>(timer_) / static_cast<float>(frame_));
				//スプライトの透明度を設定
				alpha_ = alpha;
			}
		}
		break;
	default:
		break;
	}

}

void SceneTransitionAnimation::EndIn() {
	switch (type_) {
	case SceneTransitionAnimation::TransitionType::FADE:
		//フェードイン終了
		if (state_ == TransitionState::END_IN) {
			//フェードアウト開始
			state_ = TransitionState::UPDATE_OUT;
			//フレームをリセット
			timer_ = frame_;
			//スプライトの透明度を設定
			alpha_ = 1.0f;
		}
		break;
	default:
		break;
	}
}

void SceneTransitionAnimation::UpdateOut() {
	switch (type_) {
	case SceneTransitionAnimation::TransitionType::FADE:
		//フェードアウト処理
		if (state_ == TransitionState::UPDATE_OUT) {
			//フレームが0になったら
			if (timer_ == 0) {
				//フェードアウト終了
				state_ = TransitionState::END_OUT;
			}
			else {
				//フレームを減らす
				timer_--;
				//透明度を計算
				float alpha = 1.0f - MyMath::Lerp(1.0f, 0.0f, static_cast<float>(timer_) / static_cast<float>(frame_));
				//スプライトの透明度を設定
				alpha_ = alpha;
			}
		}
		break;
	default:
		break;
	}
}

void SceneTransitionAnimation::EndOut() {
	switch (type_) {
	case SceneTransitionAnimation::TransitionType::FADE:
		//フェードアウト終了
		if (state_ == TransitionState::END_OUT) {
			//遷移終了
			state_ = TransitionState::END_ALL;
			//遷移中フラグを下げる
			isTransitioning_ = false;
			//スプライトの透明度を設定
			alpha_ = 0.0f;
		}
		break;
	default:
		break;
	}
}

void SceneTransitionAnimation::EndAll() {
	//遷移終了
	state_ = TransitionState::NONE;
	type_ = TransitionType::NONE;
	frame_ = 0;
	timer_ = 0;
	//遷移中フラグを下げる
	isTransitioning_ = false;
}

void SceneTransitionAnimation::InitD2D1() {
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory_.GetAddressOf());
	assert(SUCCEEDED(hr) && "D2D1ファクトリーの生成に失敗しました");

	// ウィンドウハンドルを取得
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	RECT rc;
	GetClientRect(hwnd, &rc);

	// RenderTargetの作成
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
	hr = d2dFactory_->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hwnd, size),
		renderTarget_.GetAddressOf()
	);
	assert(SUCCEEDED(hr) && "Hwndレンダーターゲットの生成に失敗しました");

	// 黒色ブラシの作成
	hr = renderTarget_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), blackBrush_.GetAddressOf());
	assert(SUCCEEDED(hr) && "カラーブラシの生成に失敗しました");
}

void SceneTransitionAnimation::DrawD2D1() {
	// 描画開始
	renderTarget_->BeginDraw();

	// 背景をクリア
	renderTarget_->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// 黒幕を描画
	blackBrush_->SetOpacity(alpha_);
	renderTarget_->FillRectangle(
		D2D1::RectF(0, 0, static_cast<float>(renderTarget_->GetSize().width), static_cast<float>(renderTarget_->GetSize().height)),
		blackBrush_.Get()
	);

	// 描画終了
	HRESULT hr = renderTarget_->EndDraw();
	assert(SUCCEEDED(hr) && "D2Dの描画に失敗しました");
}
