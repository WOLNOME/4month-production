#include "ClearScene.h"
#include "SceneManager.h"

void ClearScene::Initialize()
{
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

	textureHandleClear_ = TextureManager::GetInstance()->LoadTexture("Clear.png");
	spriteClear_ = std::make_unique<Sprite>();
	spriteClear_->Initialize(textureHandleClear_);

	//スペースUIテキスト
	spriteUI_SPACE_ = std::make_unique<TextWrite>();
	spriteUI_SPACE_->Initialize("SPACE");
	spriteUI_SPACE_->SetParam({ 500.0f, 475.0f }, Font::UDDegitalNK_R, 80.0f, { 1, 1, 1, 1 });
	spriteUI_SPACE_->SetEdgeParam({ 0, 0, 0, 1 }, 9.0f, 0.0f, true);


	bgm_ = std::make_unique<Audio>();
	bgm_->Initialize("gameclear/bgm.wav");
	bgm_->Play(true, 0.5f);
	tapSE_ = std::make_unique<Audio>();
	tapSE_->Initialize("soundeffect/tap.wav");
}

void ClearScene::Finalize()
{
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

	spriteClear_->Update();

#ifdef _DEBUG
	ImGui::Begin("scene");
	ImGui::Text("%s", "CLEAR");
	ImGui::Text("%s", "ToTitle : TAB");

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

	spriteClear_->Draw();

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
