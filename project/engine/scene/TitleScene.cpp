#include "TitleScene.h"
#include "SceneManager.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwrite.lib")

void TitleScene::Initialize()
{
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

	//生成と初期化
	textureHandleTitle_ = TextureManager::GetInstance()->LoadTexture("title.png");
	spriteTitle_ = std::make_unique<Sprite>();
	spriteTitle_->Initialize(textureHandleTitle_);

	//テキスト
	text = TextWriter::GetInstance();
	text->registerSolidColorBrash("UI", D2D1::ColorF::Red);
	text->registerTextFormat("UI", L"メイリオ", 32.0f);

	
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	if (input_->TriggerKey(DIK_TAB)) {
		sceneManager_->SetNextScene("GAMEPLAY");
	}

	spriteTitle_->Update();

#ifdef _DEBUG
	ImGui::Begin("scene");
	ImGui::Text("%s", "TITLE");
	ImGui::End();
#endif // _DEBUG
}

void TitleScene::Draw()
{
	//3Dモデルの共通描画設定
	Object3dCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓モデル描画開始↓↓↓↓
	///------------------------------///

	

	///------------------------------///
	///↑↑↑↑モデル描画終了↑↑↑↑
	///------------------------------///

	//パーティクルの共通描画設定
	ParticleCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓パーティクル描画開始↓↓↓↓
	///------------------------------///


	///------------------------------///
	///↑↑↑↑パーティクル描画終了↑↑↑↑
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

	spriteTitle_->Draw();

	///------------------------------///
	///↑↑↑↑スプライト描画終了↑↑↑↑
	///------------------------------///
	
}

void TitleScene::TextDraw() {

	D2D1_RECT_F rect;
	rect = { 100.0f,100.0f,500.0f,150.0f };
	text->WriteText("UI", "UI", L"テストメッセージ\n成功", rect);

}
