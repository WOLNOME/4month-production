#include "StageSelectScene.h"

#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "LineDrawerCommon.h"
#include "SpriteCommon.h"
#include "SceneManager.h"
#include <numbers>

uint32_t StageSelectScene::setStage_ = 0;

void StageSelectScene::Initialize() {
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

	//カメラの生成と初期化
	camera_ = std::make_unique<GameCamera>();
	camera_->Initialize();
	camera_->SetRotate({ cameraRotate });
	camera_->SetTranslate(cameraTranslate);
	camera_->SetFarClip(80.0f);
	//パーティクルマネージャーにカメラをセット
	ParticleManager::GetInstance()->SetCamera(camera_.get());

	// ステージ選択スプライト
	textureHandleUI_A_ = TextureManager::GetInstance()->LoadTexture("UI_A.png");
	spriteUI_A_ = std::make_unique<Sprite>();
	spriteUI_A_->Initialize(textureHandleUI_A_);

	textureHandleUI_D_ = TextureManager::GetInstance()->LoadTexture("UI_D.png");
	spriteUI_D_ = std::make_unique<Sprite>();
	spriteUI_D_->Initialize(textureHandleUI_D_);

	textureHandleUI_SELECT = TextureManager::GetInstance()->LoadTexture("UI_SELECT.png");
	spriteUI_SELECT_ = std::make_unique<Sprite>();
	spriteUI_SELECT_->Initialize(textureHandleUI_SELECT);

	textPos_ = { 5.0f,5.0f };
	for (uint32_t i = 0; i < 6; i++) {
		auto num = std::make_unique<TextWrite>();

		num->Initialize("num" + std::to_string(i));
		num->SetParam(textPos_, Font::GenEiPOPle, 80.0f, { 1, 1, 1, 1 });
		num->SetEdgeParam({ 0, 0, 0, 1 }, 9.0f, { 0.0f,0.0f }, true);

		//プッシュバック
		selectNumText_.push_back(std::move(num));
	}
	//スペースUIテキスト
	spaceText_ = std::make_unique<TextWrite>();
	spaceText_->Initialize("SPACE");
	spaceText_->SetParam({ 500.0f, 575.0f }, Font::GenEiPOPle, 80.0f, { 1, 1, 1, 1 });
	spaceText_->SetEdgeParam({ 0, 0, 0, 1 }, 9.0f, { -0.01f,0.0f }, true);

	//天球
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	// フィールドモデル
	for (uint32_t i = 0; i < stageNum_; i++) {
		auto field0 = std::make_unique<StageSelectObject>();
		field0->Initialize("cube", "grid.png", i);
		field0->SetStagePosition(setStage_);

		selectObjects_.push_back(std::move(field0));
	}

	//selectObjects_[0]->SetPosition({ 0.0f, 0.0f, 0.0f });
	//selectObjects_[1]->SetPosition({ 10.0f, 0.0f, 0.0f });
	//selectObjects_[2]->SetPosition({ 20.0f, 0.0f, 0.0f });
	//selectObjects_[3]->SetPosition({ 30.0f, 0.0f, 0.0f });
	//selectObjects_[4]->SetPosition({ 40.0f, 0.0f, 0.0f });
	//selectObjects_[5]->SetPosition({ 50.0f, 0.0f, 0.0f });
	//落下オブジェクト
	fallingObject_ = std::make_unique<FallingObject>();
	fallingObject_->Initialize();

	//パーティクル
	particle_ = std::make_unique<Particle>();
	particle_->Initialize("RainbowSnow", "RainbowSnow");
	particle_->emitter_.transform.translate = cameraTranslate;
	particle_->emitter_.transform.translate.z += 30.0f;
	particle_->emitter_.transform.translate.y += -12.0f;
	particle_->emitter_.transform.scale = { 12.0f,12.0f,6.0f };

	bgm_ = std::make_unique<Audio>();
	bgm_->Initialize("select/bgm.wav");
	bgm_->Play(true, 0.5f);

	selectSE_ = std::make_unique<Audio>();
	selectSE_->Initialize("soundeffect/select.wav");

	tapSE_ = std::make_unique<Audio>();
	tapSE_->Initialize("soundeffect/tap.wav");
}

void StageSelectScene::Finalize() {
	for (uint32_t i = 0; i < stageNum_; i++) {
		selectObjects_[i]->Finalize();
	}

	bgm_->Stop();
	bgm_.reset();
	selectSE_->Stop();
	selectSE_.reset();
	tapSE_->Stop();
	tapSE_.reset();

	if (isTransitionTitle_)
	{
		setStage_ = 0;
	}
	

}

void StageSelectScene::Update() {
	// カメラの更新
	camera_->Update();

	// ステージ選択
	SelectStageOperator();

	skydome_->Update();
	for (uint32_t i = 0; i < stageNum_; i++) {
		selectObjects_[i]->Update();
		selectObjects_[i]->SetStage(setStage_);
	}
	fallingObject_->Update();

	if (input_->TriggerKey(DIK_SPACE)) {
		tapSE_->Play(false, 0.5f);
		if (setStage_ == 0) {
			sceneManager_->SetNextScene("TUTORIAL");
		}
		else if (setStage_ == 1) {
			sceneManager_->SetNextScene("GAMEPLAY");
		}
		else if (setStage_ == 2) {
			sceneManager_->SetNextScene("GAMEPLAY2");
		}
		else if (setStage_ == 3) {
			sceneManager_->SetNextScene("GAMEPLAY3");
		}
		else if (setStage_ == 4) {
			sceneManager_->SetNextScene("GAMEPLAY4");
		}
		else if (setStage_ == 5) {
			sceneManager_->SetNextScene("GAMEPLAY5");
		}
	}

	//タイトル画面へ戻る
	if (input_->TriggerKey(DIK_ESCAPE)) {
		sceneManager_->SetNextScene("TITLE");
		isTransitionTitle_ = true;
	}

	spriteUI_A_->Update();
	spriteUI_D_->Update();
	spriteUI_SELECT_->Update();

	if (!selectObjects_[setStage_]->IsMove()) {

		drawSelectNum_ = setStage_;
	}

	for (auto& text : selectNumText_) {
		text->SetPosition(textPos_);
	}

#ifdef _DEBUG
	ImGui::Begin("scene");
	ImGui::Text("%s", "StageSelect");

	ImGui::SliderFloat3("cameraTranslate", &cameraTranslate.x, -50.0f, 50.0f);
	ImGui::SliderFloat3("cameraRotate", &cameraRotate.x, -5.0f, 5.0f);

	ImGui::Text("selectStage : %d", setStage_);
	bool isMove = selectObjects_[0]->IsMove();
	ImGui::Text("selectObjects_[0]->IsMove() : %s", isMove ? "true" : "false");

	ImGui::SliderFloat2("spritePos", &textPos_.x, 0, 100.0f);

	ImGui::End();

	for (uint32_t i = 0; i < stageNum_; i++) {
		selectObjects_[i]->ImGuiDraw();
	}

	camera_->DebugWithImGui();

	fallingObject_->DebugWithImGui();

	//UIテキスト用のImGui
	spaceText_->DebugWithImGui();

#endif // _DEBUG
}

void StageSelectScene::Draw() {
	//3Dモデルの共通描画設定
	Object3dCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓モデル描画開始↓↓↓↓
	///------------------------------///

	skydome_->Draw(*camera_);
	for (uint32_t i = 0; i < stageNum_; i++) {
		selectObjects_[i]->Draw(*camera_);
	}
	fallingObject_->Draw(*camera_);

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

	if (setStage_ != 0) {
		spriteUI_A_->Draw();
	}
	if (setStage_ != 5) {
		spriteUI_D_->Draw();
	}
	spriteUI_SELECT_->Draw();

	///------------------------------///
	///↑↑↑↑スプライト描画終了↑↑↑↑
	///------------------------------///
}

void StageSelectScene::TextDraw() {
	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///

	//スペースUIテキスト
	spaceText_->WriteText(L"SPACE");

	//セレクト番号UIテキスト
	const wchar_t* numText[] = {
		L"チュートリアル",
		L"ステージ1",
		L"ステージ2",
		L"ステージ3",
		L"ステージ4",
		L"ステージ5"
	};
	selectNumText_[drawSelectNum_]->WriteText(numText[drawSelectNum_]);


	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///
}

void StageSelectScene::SelectStageOperator() {
	if (!selectObjects_[0]->IsMove()) {
		if (input_->TriggerKey(DIK_D)) {
			if (setStage_ < stageNum_ - 1) {
				setStage_++;
			}
			selectSE_->Play(false, 0.8f);
		}

		if (input_->TriggerKey(DIK_A)) {
			if (setStage_ > 0) {
				setStage_--;
			}
			selectSE_->Play(false, 0.8f);
		}
	}
}
