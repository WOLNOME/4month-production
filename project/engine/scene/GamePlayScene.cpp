#include "GamePlayScene.h"
#include "SceneManager.h"

void GamePlayScene::Initialize() {
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

}

void GamePlayScene::Finalize() {
}

void GamePlayScene::Update() {
	//タイトルシーンに戻る
	if (input_->TriggerKey(DIK_ESCAPE)) {
		sceneManager_->SetNextScene("TITLE");
	}

#ifdef _DEBUG
	ImGui::Begin("scene");
	ImGui::Text("%s", "GAMEPLAY");
	ImGui::End();
#endif // _DEBUG
}

void GamePlayScene::Draw() {
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

	///------------------------------///
	///↑↑↑↑スプライト描画終了↑↑↑↑
	///------------------------------///
}
