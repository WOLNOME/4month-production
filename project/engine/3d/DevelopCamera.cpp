#include "DevelopCamera.h"
#include "ImGuiManager.h"
#include "Input.h"
#include <algorithm>
#include <numbers>

void DevelopCamera::Initialize() {
	//基盤の初期化
	BaseCamera::Initialize();
	//インプット
	input_ = Input::GetInstance();

}

void DevelopCamera::Update() {
#ifdef _DEBUG

	////開発用カメラのマウス操作処理
	//スクロールで前進後退
	standardPosition += GetForwardDirection() * (input_->GetMouseScrollCount() * 1.3f);
	//ドラッグアンドドロップでカメラの向きを変える
	if (input_->PushMouseButton(MouseButton::RightButton)) {
		//マウスの移動幅
		Vector2 moveValue = input_->GetMousePosition();
		//デッドゾーン
		float deadZone = 2.0f;
		if (moveValue.Length() > deadZone) {
			transform.rotate.x += moveValue.y * 0.001f;
			transform.rotate.y += moveValue.x * 0.001f;
		}

		//WASDでカメラ移動
		if (input_->PushKey(DIK_W)) {
			standardPosition += GetForwardDirection() * 0.1f;
		}
		if (input_->PushKey(DIK_A)) {
			standardPosition += GetLeftDirection() * 0.1f;
		}
		if (input_->PushKey(DIK_S)) {
			standardPosition += GetBackDirection() * 0.1f;
		}
		if (input_->PushKey(DIK_D)) {
			standardPosition += GetRightDirection() * 0.1f;
		}


	}
	//カメラの回転制限
	const float maxPitch = (std::numbers::pi_v<float> / 2.0f) - 0.01f;
	transform.rotate.x = std::clamp(transform.rotate.x, -maxPitch, maxPitch);

#endif // _DEBUG

	//行列の更新
	BaseCamera::UpdateMatrix();

}
