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

	//スペースUIテキスト
	spriteUI_SPACE_ = std::make_unique<TextWrite>();
	spriteUI_SPACE_->Initialize("SPACE");
	spriteUI_SPACE_->SetParam({ 500.0f, 475.0f }, Font::UDDegitalNK_R, 80.0f, { 1, 1, 1, 1 });
	spriteUI_SPACE_->SetEdgeParam({ 0, 0, 0, 1 }, 9.0f, 0.0f, true);

	bgm_ = std::make_unique<Audio>();
	bgm_->Initialize("gameover/bgm.wav");
	bgm_->Play(true, 0.5f);
}

void GameOverScene::Finalize()
{
	bgm_->Stop();
	bgm_.reset();
}

void GameOverScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE))
	{
		sceneManager_->SetNextScene("TITLE");
	}

	spriteGO_->Update();

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
