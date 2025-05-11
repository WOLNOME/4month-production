#include "GameOverScene.h"

#include "SceneManager.h"

void GameOverScene::Initialize()
{
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

	//カメラの生成と初期化
	camera = std::make_unique<DevelopCamera>();
	camera->Initialize();
	camera->SetTranslate(cameraTranslate); // カメラの位置を調整
	camera->SetRotate({ 0.0f, 0.0f, 0.0f }); // カメラの回転を調整
	//パーティクルマネージャーにカメラをセット
	ParticleManager::GetInstance()->SetCamera(camera.get());

	textureHandleFall1_ = TextureManager::GetInstance()->LoadTexture("fall1.png");
	spriteFall1_ = std::make_unique<Sprite>();
	spriteFall1_->Initialize(textureHandleFall1_);
	spriteFall1_->SetPosition({ 0.0f, spriteFall1Y_ });


	textureHandleFall2_ = TextureManager::GetInstance()->LoadTexture("fall2.png");
	spriteFall2_ = std::make_unique<Sprite>();
	spriteFall2_->Initialize(textureHandleFall2_);
	spriteFall2_->SetPosition({ 0.0f, spriteFall2Y_ });

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
	field_->SetTextureHandle("GameOver.png");
	field_->SetScale({ 26.0f, 1.0f,26.0f });
	field_->SetRotation({ -1.57f, 0.0f, 0.0f });

	bgm_ = std::make_unique<Audio>();
	bgm_->Initialize("gameover/bgm.wav");
	bgm_->Play(true, 0.5f);

	tapSE_ = std::make_unique<Audio>();
	tapSE_->Initialize("soundeffect/tap.wav");
}

void GameOverScene::Finalize()
{
	field_->Finalize();
	field_.reset();

	bgm_->Stop();
	bgm_.reset();
	tapSE_->Stop();
	tapSE_.reset();
}

void GameOverScene::Update()
{

	//カメラの更新
	camera->Update();
	camera->SetTranslate(cameraTranslate);

	if (!isAnimationComplete_) {
		cameraTranslate.z -= 1.0f;
	}


	//スカイドームの更新
	skydome_->Update();

	//フィールドの更新
	field_->Update();

	if (input_->TriggerKey(DIK_SPACE))
	{
		tapSE_->Play(false, 0.5f);
		sceneManager_->SetNextScene("TITLE");
	}

	spriteFall1_->Update();
	spriteFall2_->Update();

	if (cameraTranslate.z <= -300.0f)
	{
		isAnimationStert_ = true;
	}

	EyeClose();

#ifdef _DEBUG
	ImGui::Begin("scene");

	ImGui::Text("%s", "GameOver");
	ImGui::Text("%s", "ToTitle : TAB");

	/*======カメラ========*/
	//位置
	Vector3 cameraTranslate = camera->GetTranslate();
	ImGui::DragFloat3("cameraTranslate", &cameraTranslate.x, 0.1f);
	camera->SetTranslate(cameraTranslate);
	//回転
	Vector3 cameraRotate = camera->GetRotate();
	ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.1f);
	camera->SetRotate(cameraRotate);


	/*======フィールド======*/
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

	ImGui::End();
#endif // _DEBUG
}

void GameOverScene::Draw()
{
	//3Dモデルの共通描画設定
	Object3dCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓モデル描画開始↓↓↓↓
	///------------------------------///

	// スカイドーム
	skydome_->Draw(*camera.get());
	// フィールド
	field_->Draw(*camera.get());

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

	 
	spriteFall1_->Draw();
	spriteFall2_->Draw();
	

	///------------------------------///
	///↑↑↑↑スプライト描画終了↑↑↑↑
	///------------------------------///
}

void GameOverScene::TextDraw()
{
	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///


	//スペースUIテキスト
	spriteUI_SPACE_->WriteText(L"SPACE");


	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///
}

void GameOverScene::EyeClose()
{
	if (isAnimationComplete_) {
		return; // 演出が完了している場合は何もしない
	}

	if (isAnimationStert_)
	{

		switch (closeState_) {
		case 0: // 初期状態
			closeState_ = 1; // 速く閉じる状態に移行
			break;

		case 1: // 速く閉じる
			spriteFall1Y_ += fallSpeed_;
			spriteFall2Y_ -= fallSpeed_;
			spriteFall1_->SetPosition({ 0.0f, spriteFall1Y_ });
			spriteFall2_->SetPosition({ 0.0f, spriteFall2Y_ });

			if (spriteFall1Y_ >= 0.0f && spriteFall2Y_ <= 360.0f) {
				closeState_ = 2; // 開く状態に移行
				fallSpeed_ = 30.0f; // 開く速度を設定
			}
			break;

		case 2: // 開く
			spriteFall1Y_ -= fallSpeed_;
			spriteFall2Y_ += fallSpeed_;
			spriteFall1_->SetPosition({ 0.0f, spriteFall1Y_ });
			spriteFall2_->SetPosition({ 0.0f, spriteFall2Y_ });

			if (spriteFall1Y_ <= -720.0f && spriteFall2Y_ >= 720.0f) {
				closeState_ = 3; // ゆっくり閉じる状態に移行
				fallSpeed_ = 5.0f; // ゆっくり閉じる速度を設定
			}
			break;

		case 3: // ゆっくり閉じる
			spriteFall1Y_ += fallSpeed_;
			spriteFall2Y_ -= fallSpeed_;
			spriteFall1_->SetPosition({ 0.0f, spriteFall1Y_ });
			spriteFall2_->SetPosition({ 0.0f, spriteFall2Y_ });

			if (spriteFall1Y_ >= 0.0f && spriteFall2Y_ <= 720.0f) {
				isAnimationComplete_ = true; // 演出完了
			}
			break;
		}

	}
}
