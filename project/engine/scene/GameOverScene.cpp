#include "GameOverScene.h"

#include "SceneManager.h"

void GameOverScene::Initialize()
{
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

	textureHandleGO_ = TextureManager::GetInstance()->LoadTexture("GameOver.png");
	spriteGO_ = std::make_unique<Sprite>();
	spriteGO_->Initialize(textureHandleGO_);

	textureHandleUI_SPACE_ = TextureManager::GetInstance()->LoadTexture("UI_SPACE.png");
	spriteUI_SPACE_ = std::make_unique<Sprite>();
	spriteUI_SPACE_->Initialize(textureHandleUI_SPACE_);

}

void GameOverScene::Finalize()
{
}

void GameOverScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE))
	{
		sceneManager_->SetNextScene("TITLE");
	}

	spriteGO_->Update();
	spriteUI_SPACE_->Update();

#ifdef _DEBUG
	ImGui::Begin("scene");
	ImGui::Text("%s", "GameOver");
	ImGui::Text("%s", "ToTitle : TAB");

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

	spriteGO_->Draw();
	spriteUI_SPACE_->Draw();

	///------------------------------///
	///↑↑↑↑スプライト描画終了↑↑↑↑
	///------------------------------///
}

void GameOverScene::TextDraw()
{
	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///



	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///
}
