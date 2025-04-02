#include "StageSelectScene.h"

#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "LineDrawerCommon.h"
#include "SpriteCommon.h"
#include "SceneManager.h"
#include <numbers>

void StageSelectScene::Initialize()
{
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

	//カメラの生成と初期化
	camera_ = std::make_unique<BaseCamera>();
	camera_->Initialize();
	camera_->SetRotate({ cameraRotate });
	camera_->SetTranslate(cameraTranslate);
	camera_->SetFarClip(80.0f);
	//パーティクルマネージャーにカメラをセット
	ParticleManager::GetInstance()->SetCamera(camera_.get());

	// ステージ選択スプライト
	textureHandleSelect_ = TextureManager::GetInstance()->LoadTexture("UI_SPACE.png");
	spriteSelect_ = std::make_unique<Sprite>();
	spriteSelect_->Initialize(textureHandleSelect_);

	textureHandleUI_A_ = TextureManager::GetInstance()->LoadTexture("UI_A.png");
	spriteUI_A_ = std::make_unique<Sprite>();
	spriteUI_A_->Initialize(textureHandleUI_A_);

	textureHandleUI_D_ = TextureManager::GetInstance()->LoadTexture("UI_D.png");
	spriteUI_D_ = std::make_unique<Sprite>();
	spriteUI_D_->Initialize(textureHandleUI_D_);

	spritePos_ = { 0.0f,0.0f };
	textureHandleSelectNum_.push_back(TextureManager::GetInstance()->LoadTexture("stageNum1.png"));
	textureHandleSelectNum_.push_back(TextureManager::GetInstance()->LoadTexture("stageNum2.png"));
	textureHandleSelectNum_.push_back(TextureManager::GetInstance()->LoadTexture("stageNum3.png"));
	textureHandleSelectNum_.push_back(TextureManager::GetInstance()->LoadTexture("stageNum4.png"));
	textureHandleSelectNum_.push_back(TextureManager::GetInstance()->LoadTexture("stageNum5.png"));
	for (uint32_t i = 0; i < 5; i++)
	{
		auto num = std::make_unique<Sprite>();

		num->SetPosition(spritePos_);
		num->Initialize(textureHandleSelectNum_[i]);

		spriteSelectNum_.push_back(std::move(num));
	}

	// フィールドモデル
	for (uint32_t i = 0; i < stageNum_; i++)
	{
		auto field0 = std::make_unique<StageSelectObject>();
		field0->Initialize("cube", "grid.png");

        selectObjects_.push_back(std::move(field0));
	}

	selectObjects_[0]->SetPosition({ 0.0f, 0.0f, 0.0f });
	selectObjects_[1]->SetPosition({ 10.0f, 0.0f, 0.0f });
	selectObjects_[2]->SetPosition({ 20.0f, 0.0f, 0.0f });
	selectObjects_[3]->SetPosition({ 30.0f, 0.0f, 0.0f });
	selectObjects_[4]->SetPosition({ 40.0f, 0.0f, 0.0f });

	selectStage_ = 0;

	//パーティクル
	particle_ = std::make_unique<Particle>();
	particle_->Initialize("RainbowSnow", "RainbowSnow");
	particle_->emitter_.transform.translate = cameraTranslate;
	particle_->emitter_.transform.translate.z += 10.0f;
	particle_->emitter_.transform.translate.y += -8.0f;
	particle_->emitter_.transform.scale = { 30.0f,30.0f,20.0f };

}

void StageSelectScene::Finalize()
{
	for (uint32_t i = 0; i < stageNum_; i++)
	{
		selectObjects_[i]->Finalize();
	}
}

void StageSelectScene::Update()
{
	// カメラの更新
	camera_->UpdateMatrix();
	camera_->SetRotate({ cameraRotate });
	camera_->SetTranslate(cameraTranslate);

	// ステージ選択
	StageSelect();

	for (uint32_t i = 0; i < stageNum_; i++)
	{
		selectObjects_[i]->Update();
		selectObjects_[i]->SetStage(selectStage_);
	}

	if (input_->TriggerKey(DIK_SPACE))
	{
		if (selectStage_ == 0)
		{
			sceneManager_->SetNextScene("GAMEPLAY");
		} 
		else if (selectStage_ == 1)
		{
			sceneManager_->SetNextScene("GAMEPLAY2");
		}
		else if (selectStage_ == 2)
		{
			sceneManager_->SetNextScene("GAMEPLAY3");
		}
		else if (selectStage_ == 3)
		{
			sceneManager_->SetNextScene("GAMEPLAY4");
		} 
		else if (selectStage_ == 4)
		{
			sceneManager_->SetNextScene("GAMEPLAY5");
		}

	}

	spriteSelect_->Update();
	spriteUI_A_->Update();
	spriteUI_D_->Update();

	for (auto& sprite : spriteSelectNum_)
	{
		sprite->Update();
		sprite->SetPosition(spritePos_);
	}

#ifdef _DEBUG
	ImGui::Begin("scene");
	ImGui::Text("%s", "StageSelect");

	ImGui::SliderFloat3("cameraTranslate", &cameraTranslate.x, -50.0f, 50.0f);
	ImGui::SliderFloat3("cameraRotate", &cameraRotate.x, -5.0f, 5.0f);

	ImGui::Text("selectStage : %d", selectStage_);
	bool isMove = selectObjects_[0]->IsMove();
	ImGui::Text("selectObjects_[0]->IsMove() : %s", isMove ? "true" : "false");

	ImGui::SliderFloat2("spritePos", &spritePos_.x, 0, 100.0f);

	ImGui::End();

	for (uint32_t i = 0; i < stageNum_; i++)
	{
		selectObjects_[i]->ImGuiDraw();
	}

#endif // _DEBUG
}

void StageSelectScene::Draw()
{
	//3Dモデルの共通描画設定
	Object3dCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓モデル描画開始↓↓↓↓
	///------------------------------///

	for (uint32_t i = 0; i < stageNum_; i++)
	{
		selectObjects_[i]->Draw(*camera_);
	}

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

	spriteSelect_->Draw();

	if (selectStage_ != 0)
	{
		spriteUI_A_->Draw();
	}
	if (selectStage_ != 4)
	{
		spriteUI_D_->Draw();
	}

	spriteSelectNum_[selectStage_]->Draw();

	///------------------------------///
	///↑↑↑↑スプライト描画終了↑↑↑↑
	///------------------------------///
}

void StageSelectScene::TextDraw()
{
	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///



	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///
}

void StageSelectScene::StageSelect()
{
	if (!selectObjects_[0]->IsMove())
	{
		if (input_->TriggerKey(DIK_D))
		{
			if (selectStage_ < stageNum_ - 1)
			{
				selectStage_++;
			}
		}

		if (input_->TriggerKey(DIK_A))
		{
			if (selectStage_ > 0)
			{
				selectStage_--;
			}
		}
	}
}
