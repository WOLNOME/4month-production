#include "ClearScene.h"
#include "SceneManager.h"

#include <MyMath.h>

void ClearScene::Initialize()
{
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

	//カメラの生成と初期化
	camera = std::make_unique<DevelopCamera>();
	camera->Initialize(); 
	cameraTranslate = playerPosition_ + Vector3(-10.0f, 1.5f, -5.0f); // 左側
	cameraRotate = { 0.25f, 0.5f, 0.0f }; // プレイヤーを向く回転
	camera->SetTranslate(cameraTranslate);
	camera->SetRotate(cameraRotate);
	//パーティクルマネージャーにカメラをセット
	ParticleManager::GetInstance()->SetCamera(camera.get());

	textureHandleClear_ = TextureManager::GetInstance()->LoadTexture("Clear.png");
	spriteClear_ = std::make_unique<Sprite>();
	spriteClear_->Initialize(textureHandleClear_);

	// スプライトのアンカーポイントを中心に設定
	spriteClear_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteClear_->SetPosition({ 640.0f, 360.0f });

	//スペースUIテキスト
	spriteUI_SPACE_ = std::make_unique<TextWrite>();
	spriteUI_SPACE_->Initialize("SPACE");
	spriteUI_SPACE_->SetParam({ 500.0f, 475.0f }, Font::UDDegitalNK_R, 80.0f, { 1, 1, 1, 1 });
	spriteUI_SPACE_->SetEdgeParam({ 0, 0, 0, 1 }, 9.0f, 0.0f, true);

	// スカイドーム
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();
	
	// フィールド
	field_ = std::make_unique<Field>();
	field_->Initialize();
	field_->SetTextureHandle("grid.png");
	field_->SetScale({ 26.0f, 1.0f,26.0f });
	
	// プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->SetPosition({ 0.0f, 1.0f, 0.0f });
	player_->SetRotation({ 0.0f, 3.14f, 0.0f });
	player_->IsMoveable(false);

	//bgm
	bgm_ = std::make_unique<Audio>();
	bgm_->Initialize("gameclear/bgm.wav");
	bgm_->Play(true, 0.5f);
	tapSE_ = std::make_unique<Audio>();
	tapSE_->Initialize("soundeffect/tap.wav");
}

void ClearScene::Finalize()
{
	field_->Finalize();
	field_.reset();

	player_->Finalize();
	player_.reset();

	bgm_->Stop();
	bgm_.reset();
	tapSE_->Stop();
	tapSE_.reset();
}

void ClearScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE)) {
		tapSE_->Play(false, 0.5f);
		sceneManager_->SetNextScene("TITLE");
	}

	//カメラの更新
	camera->Update();
	camera->SetTranslate(cameraTranslate);

	spriteClear_->Update();

	//スカイドームの更新
	skydome_->Update();

	//フィールドの更新
	field_->Update();

	//プレイヤーの更新
	player_->SetIsGround(true); // 地面にいると仮定
	player_->UpdateModel();

	CameraEffect();

	if (!isCameraAnimating_) {
		PlayerMotion();
	}

	ClearTextureUpdate();

#ifdef _DEBUG
	ImGui::Begin("scene");
	ImGui::Text("%s", "CLEAR");
	ImGui::Text("%s", "ToTitle : TAB");

	/*======カメラ========*/
	ImGui::Text("camera");
	//位置
	ImGui::DragFloat3("cameraTranslate", &cameraTranslate.x, 0.1f);
	camera->SetTranslate(cameraTranslate);
	//回転
	ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.1f);
	camera->SetRotate(cameraRotate);

	/*======フィールド======*/
	ImGui::Text("field");
	//フィールドの位置
	Vector3 fieldPos = field_->GetPosition();
	ImGui::DragFloat3("fieldTranslate", &fieldPos.x, 0.1f);
	field_->SetPosition(fieldPos);
	//フィールドのスケール
	Vector3 fieldScale = field_->GetScale();
	ImGui::DragFloat3("fieldScale", &fieldScale.x, 0.1f);
	field_->SetScale(fieldScale);
	//フィールドの回転
	Vector3 fieldRotate = field_->GetRotation();
	ImGui::DragFloat3("fieldRotate", &fieldRotate.x, 0.1f);
	field_->SetRotation(fieldRotate);

	/*======プレイヤー======*/
	ImGui::Text("player");
	//プレイヤーの位置
	Vector3 playerPos = player_->GetPosition();
	ImGui::DragFloat3("playerTranslate", &playerPos.x, -10.0f, 10.0f);
	player_->SetPosition(playerPos);
	// プレイヤーの回転
	Vector3 playerRotate = player_->GetRotation();
	ImGui::DragFloat3("playerRotate", &playerRotate.x, -3.14f, 3.14f);
	player_->SetRotation(playerRotate);

	// isReturning_ フラグの表示
	ImGui::Checkbox("isReturning_", &isReturning_);



	ImGui::End();
#endif // _DEBUG
}

void ClearScene::Draw()
{
	//3Dモデルの共通描画設定
	Object3dCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓モデル描画開始↓↓↓↓
	///------------------------------///

	// スカイドーム
	skydome_->Draw(*camera);
	// フィールド
	field_->Draw(*camera);
	// プレイヤー
	player_->Draw(*camera);

	///------------------------------///
	///↑↑↑↑モデル描画終了↑↑↑↑
	///------------------------------///


	//線描画共通描画設定
	LineDrawerCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓線描画開始↓↓↓↓
	///------------------------------///



	///------------------------------///
	///↑↑↑↑線描画終了↑↑↑↑
	///------------------------------///

	//スプライトの共通描画設定
	SpriteCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓スプライト描画開始↓↓↓↓
	///------------------------------///

	if (isSpriteAnimating_ or isSpriteAnimationCompleted_) {
		spriteClear_->Draw();
	}

	///------------------------------///
	///↑↑↑↑スプライト描画終了↑↑↑↑
	///------------------------------///
}

void ClearScene::TextDraw() {
	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///

	//スペースUIテキスト
	spriteUI_SPACE_->WriteText(L"SPACE");
	

	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///
}

void ClearScene::CameraEffect()
{
	if (!isCameraAnimating_) {
		return; // アニメーションが終了している場合は何もしない
	}

	// アニメーションの進行度を更新
	cameraAnimationProgress_ += 0.01f; // アニメーション速度を調整 (0.01f は進行速度)

	if (cameraAnimationProgress_ > 1.0f) {
		cameraAnimationProgress_ = 0.0f; // 進行度をリセット

		if (cameraAnimationPhase_ == 0) {
			// 側面から正面への移動が完了したら右側からの移動に移行
			cameraAnimationPhase_ = 1;
		} else if (cameraAnimationPhase_ == 1) {
			// 右側から正面への移動が完了したらカメラを引くフェーズに移行
			cameraAnimationPhase_ = 2;
		} else if (cameraAnimationPhase_ == 2) {
			// カメラを引くフェーズが完了したらアニメーション終了
			isCameraAnimating_ = false;
			return;
		}
	}

	// カメラの位置と回転を補間
	if (cameraAnimationPhase_ == 0) {
		// 側面から正面への移動
		cameraTranslate = MyMath::Lerp(playerPosition_ + Vector3(-10.0f, 3.0f, -5.0f), playerPosition_ + Vector3(0.0f, 3.0f, -10.0f), cameraAnimationProgress_);
		cameraRotate = MyMath::Lerp(Vector3(0.25f, 0.5f, 0.0f), Vector3(0.25f, 0.0f, 0.0f), cameraAnimationProgress_);
	} else if (cameraAnimationPhase_ == 1) {
		// 右側から正面への移動
		cameraTranslate = MyMath::Lerp(playerPosition_ + Vector3(10.0f, 3.0f, -5.0f), playerPosition_ + Vector3(0.0f, 3.0f, -10.0f), cameraAnimationProgress_);
		cameraRotate = MyMath::Lerp(Vector3(0.25f, -0.5f, 0.0f), Vector3(0.25f, 0.0f, 0.0f), cameraAnimationProgress_);
	} else if (cameraAnimationPhase_ == 2) {
		// カメラを引く
		cameraTranslate = MyMath::Lerp(playerPosition_ + Vector3(0.0f, 7.5f, -10.0f), Vector3(0.0f, 7.5f, -22.0f), cameraAnimationProgress_);
		cameraRotate = Vector3(0.25f, 0.0f, 0.0f); // 回転はそのまま
	}


	// カメラの位置と回転を適用
	camera->SetTranslate(cameraTranslate);
	camera->SetRotate(cameraRotate);
}

void ClearScene::PlayerMotion()
{ 

	// ジャンプの進行度を更新
	if (isPlayerJumpingUp_) {
		playerJumpProgress_ += playerJumpSpeed_;
		if (playerJumpProgress_ >= 1.0f) {
			playerJumpProgress_ = 1.0f;
			isPlayerJumpingUp_ = false; // 上昇が終わったら下降に切り替え
		}

		if (!isReturning_) {

			player_->SetRotation({ 0.0f,3.14f,0.0f });
		}
	} else {
		playerJumpProgress_ -= playerJumpSpeed_;
		if (playerJumpProgress_ <= 0.0f) {
			playerJumpProgress_ = 0.0f;
			isPlayerJumpingUp_ = true; // 下降が終わったら上昇に切り替え

			// ジャンプ終了時にスプライトアニメーションを開始
			isSpriteAnimating_ = true;
			spriteAnimationProgress_ = 0.0f;
		}
	}

	// ジャンプの高さを計算
	float jumpOffset = sinf(playerJumpProgress_ * 3.14159f) * playerJumpHeight_;

	// プレイヤーの位置を更新
	Vector3 playerPos = player_->GetPosition();
	playerPos.y = 1.3f + jumpOffset; // 基本の高さ (1.3f) にジャンプのオフセットを加える
	player_->SetPosition(playerPos);

	// プレイヤーの回転を更新
	Vector3 playerRot = player_->GetRotation();
	

	if (isPlayerJumpingUp_) {
		if (!isReturning_ && playerJumpProgress_ <= 0.2f) {
			// ジャンプの瞬間に素早く角度を変える (0.0f～0.2fの間で補間)
			playerRot.x = MyMath::Lerp(0.0f, -0.523599f, playerJumpProgress_ / 0.2f); // 30度 (ラジアン)
			if (playerRot.x <= -0.49f) {
				isReturning_ = true; // だいたい30度に達したら元に戻す処理を開始
			}
		} else if (isReturning_) {

			if (progress_ >= 1.0f) {
				
				progress_ = 0.0f;
				isReturning_ = false; // 元に戻す処理が終わったらフラグをリセット
				isPlayerJumpingUp_ = false;
			} else {
				// 元に戻す処理
				playerRot.x = MyMath::Lerp(-0.49f, 0.0f, progress_);
			}

			progress_ += 0.05f;
		}
	} else {
		// 下降中は角度を固定
		playerRot.x = 0.0f;
	}

	player_->SetRotation(playerRot);

}

void ClearScene::ClearTextureUpdate()
{
	// スプライトアニメーションの更新
	if (isSpriteAnimating_ && !isSpriteAnimationCompleted_) {
		spriteAnimationProgress_ += 0.05f; // アニメーション速度を調整

		// スプライトのスケールを「ぽよよん」と変化させる
		float baseWidth = 1280.0f;
		float baseHeight = 720.0f;

		// 余韻の回数に応じて拡大率を半減させる
		static int remainingBounces = 3; // 余韻の回数
		static float bounceFactor = 1.0f; // 拡大率の初期値
		float scaleFactor = 1.0f + sinf(spriteAnimationProgress_ * 3.14159f) * 0.5f * bounceFactor;

		spriteClear_->SetSize({ baseWidth * scaleFactor, baseHeight * scaleFactor });

		// アニメーションが終了したら余韻を持たせる
		if (spriteAnimationProgress_ >= 1.0f) {
			spriteAnimationProgress_ = 0.0f;

			if (remainingBounces > 0) {
				remainingBounces--;
				bounceFactor *= 0.5f; // 拡大率を半減
			} else {
				remainingBounces = 3; // 余韻の回数をリセット
				bounceFactor = 1.0f; // 拡大率をリセット
				spriteClear_->SetSize({ baseWidth, baseHeight }); // スケールを元に戻す
				isSpriteAnimating_ = false; // アニメーションを停止
				isSpriteAnimationCompleted_ = true; // 処理を完了状態に設定
			}
		}
	}
}
