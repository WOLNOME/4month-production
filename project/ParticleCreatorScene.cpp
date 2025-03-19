#include "ParticleCreatorScene.h"
#include "SceneManager.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "JsonUtil.h"

void ParticleCreatorScene::Initialize() {
	//シーン共通の初期化
	BaseScene::Initialize();

	//インプット
	input_ = Input::GetInstance();

	//カメラの生成と初期化
	camera_ = std::make_unique<DevelopCamera>();
	camera_->Initialize();
	camera_->SetTranslate({ 0.0f,50.0f,0.0f });
	camera_->SetRotate({ 0.2f,0.0f,0.0f });

	//天球と地面の生成と初期化
	wtSkydome_.Initialize();
	skydome_ = std::make_unique<Object3d>();
	skydome_->InitializeModel("skydome");
	wtGround_.Initialize();
	textureHandleGround_ = TextureManager::GetInstance()->LoadTexture("grid.png");
	ground_ = std::make_unique<Object3d>();
	ground_->InitializeModel("ground");
}

void ParticleCreatorScene::Finalize() {
}

void ParticleCreatorScene::Update() {
	//シーン共通の更新
	BaseScene::Update();
	//カメラの更新
	camera_->Update();
	//モデルの更新
	wtSkydome_.UpdateMatrix();
	wtGround_.UpdateMatrix();

	//リセットコマンド
	if (input_->TriggerKey(DIK_ESCAPE)) {
		isReset_ = true;
	}


	//ImGui操作
#ifdef _DEBUG
	//最初の操作
	StartWithImGui();
	//新規作成の操作
	GenerateWithImGui();
	//編集の操作
	EditWithImGui();

	//リセット処理
	if (isReset_) {
		ImGui::OpenPopup("確認");
	}
	if (ImGui::BeginPopupModal("確認", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("リセットしますか？");
		if (ImGui::Button("はい", ImVec2(120, 0))) {
			sceneManager_->SetNextScene("PARTICLECREATOR");
			isReset_ = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("いいえ", ImVec2(120, 0))) {
			isReset_ = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
#endif // _DEBUG

}

void ParticleCreatorScene::Draw() {
	//3Dモデルの共通描画設定
	Object3dCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓モデル描画開始↓↓↓↓
	///------------------------------///

	//天球の描画
	skydome_->Draw(wtSkydome_, *camera_.get(), nullptr);
	//地面の描画
	ground_->Draw(wtGround_, *camera_.get(), nullptr, textureHandleGround_);

	///------------------------------///
	///↑↑↑↑モデル描画終了↑↑↑↑
	///------------------------------///

}

void ParticleCreatorScene::TextDraw() {
}

void ParticleCreatorScene::StartWithImGui() {
#ifdef _DEBUG
	//最初の操作
	if (!isGenerateMode_ && !isEditMode_) {
		ImGui::Begin("メニュー");
		ImGui::Text("パーティクルエディタへようこそ！\n以下の項目から希望のオプションを選択して下さい。\n");
		if (ImGui::Button("新しくパーティクルを作成する")) {
			isGenerateMode_ = true;
			//パーティクルの生成

			//基本パーティクルをロード

			//jsonデータをロード

			//カメラの位置をセット

		}
		if (ImGui::Button("既存のパーティクルを編集する")) {
			isEditMode_ = true;
		}
		ImGui::End();
	}

#endif // _DEBUG
}

void ParticleCreatorScene::GenerateWithImGui() {
#ifdef _DEBUG
	//新規作成の操作
	if (isGenerateMode_ && !isEditMode_) {
		ImGui::Begin("新規作成");

		ImGui::End();
	}

#endif // _DEBUG
}

void ParticleCreatorScene::EditWithImGui() {
}
