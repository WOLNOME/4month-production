#include "ParticleCreatorScene.h"
#include "SceneManager.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "ParticleManager.h"
#include "JsonUtil.h"
#include <d3d11.h>
#include "DirectXTex.h"
#include <filesystem>
#include <iostream>

#pragma comment(lib,"DirectXTex.lib")



extern ID3D11Device* g_pd3dDevice;


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
	//パーティクルマネージャーにカメラをセット
	ParticleManager::GetInstance()->SetCamera(camera_.get());

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
	ResetWithImGui();

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
		ImGui::SetNextWindowPos(ImVec2(450, 300), ImGuiCond_FirstUseEver);
		ImGui::Begin("メニュー");
		ImGui::Text("パーティクルエディターへようこそ！\n以下の項目から希望のオプションを選択して下さい。\n");
		if (ImGui::Button("新しくパーティクルを作成する")) {
			isGenerateMode_ = true;
			//パーティクルの生成
			particle_ = std::make_unique<Particle>();
			//基本パーティクルをロード
			particle_->Initialize("Sample", "Basic");
			//エミッターの位置を調整
			particle_->emitter_.transform.translate = { 0.0f,3.0f,0.0f };
			//jsonデータをロード
			editParam_ = particle_->GetParam();
			//カメラの位置をセット
			camera_->SetTranslate({ 0.0f,4.0f,-20.0f });
			camera_->SetRotate({ 0.03f,0.0f,0.0f });
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
	if (isGenerateMode_ && !isEditMode_) {
		//パラメーター
		ImGui::SetNextWindowPos(ImVec2(10, 80), ImGuiCond_FirstUseEver);
		ImGui::Begin("パーティクルのパラメーター");
		//テクスチャ
		std::string selectedTexture = particle_->GetParam()["Texture"];
		if (ImGui::CollapsingHeader("テクスチャの設定")) {
			if (ImGui::Button("{textures}フォルダ内のテクスチャを検索")) {
				showFileDialog_ = true;
				textureFiles_.clear();
				for (const auto& entry : std::filesystem::directory_iterator("Resources/textures")) {
					if (entry.is_regular_file() && entry.path().extension() == ".png") {
						textureFiles_.push_back(entry.path().filename().string());
					}
				}
			}

			if (showFileDialog_) {
				ImGui::SetNextWindowPos(ImVec2(360, 80));
				ImGui::Begin("テクスチャ一覧");
				for (const auto& file : textureFiles_) {
					if (ImGui::Selectable(file.c_str())) {
						selectedTexture = file;  // 選択したファイル名を保存
						showFileDialog_ = false;  // ウィンドウを閉じる
					}
				}
				ImGui::End();
			}

			ImGui::Text("選択中のテクスチャ : %s", selectedTexture.c_str());
			ID3D11ShaderResourceView* texture = nullptr;
			DirectX::ScratchImage image;
			std::string fileName = "Resources/textures/" + selectedTexture;
			HRESULT hr = DirectX::LoadFromWICFile(
				std::wstring(fileName.begin(), fileName.end()).c_str(),
				DirectX::WIC_FLAGS_NONE,
				nullptr,
				image
			);
			if (SUCCEEDED(hr)) {
				

			}

		}

		//色を写す
		Vector4 startColorMax = { editParam_["StartColor"]["Max"]["x"],editParam_["StartColor"]["Max"]["y"],editParam_["StartColor"]["Max"]["z"],editParam_["StartColor"]["Max"]["w"] };
		Vector4 startColorMin = { editParam_["StartColor"]["Min"]["x"],editParam_["StartColor"]["Min"]["y"],editParam_["StartColor"]["Min"]["z"],editParam_["StartColor"]["Min"]["w"] };
		Vector4 endColorMax = { editParam_["EndColor"]["Max"]["x"],editParam_["EndColor"]["Max"]["y"],editParam_["EndColor"]["Max"]["z"],editParam_["EndColor"]["Max"]["w"] };
		Vector4 endColorMin = { editParam_["EndColor"]["Min"]["x"],editParam_["EndColor"]["Min"]["y"],editParam_["EndColor"]["Min"]["z"],editParam_["EndColor"]["Min"]["w"] };
		if (ImGui::CollapsingHeader("色の設定")) {
			ImGui::ColorEdit4("StartColorMax", &startColorMax.x);
			ImGui::ColorEdit4("StartColorMin", &startColorMin.x);
			ImGui::ColorEdit4("EndColorMax", &endColorMax.x);
			ImGui::ColorEdit4("EndColorMin", &endColorMin.x);
		}
		//サイズを写す
		float startSizeMax = editParam_["StartSize"]["Max"];
		float startSizeMin = editParam_["StartSize"]["Min"];
		float endSizeMax = editParam_["EndSize"]["Max"];
		float endSizeMin = editParam_["EndSize"]["Min"];
		if (ImGui::CollapsingHeader("サイズの設定")) {
			ImGui::SliderFloat("StartSizeMax", &startSizeMax, startSizeMin, 10.0f);
			ImGui::SliderFloat("StartSizeMin", &startSizeMin, 0.0f, startSizeMax);
			ImGui::SliderFloat("EndSizeMax", &endSizeMax, endSizeMin, 10.0f);
			ImGui::SliderFloat("EndSizeMin", &endSizeMin, 0.0f, endSizeMax);
		}
		//速度を写す
		Vector3 velocityMax = { editParam_["Velocity"]["Max"]["x"],editParam_["Velocity"]["Max"]["y"],editParam_["Velocity"]["Max"]["z"] };
		Vector3 velocityMin = { editParam_["Velocity"]["Min"]["x"],editParam_["Velocity"]["Min"]["y"],editParam_["Velocity"]["Min"]["z"] };
		if (ImGui::CollapsingHeader("速度の設定")) {
			ImGui::DragFloat3("VelocityMax", &velocityMax.x, 0.1f);
			ImGui::DragFloat3("VelocityMin", &velocityMin.x, 0.1f);
		}
		//寿命を写す
		float lifeTimeMax = editParam_["LifeTime"]["Max"];
		float lifeTimeMin = editParam_["LifeTime"]["Min"];
		if (ImGui::CollapsingHeader("寿命の設定")) {
			ImGui::SliderFloat("LifeTimeMax", &lifeTimeMax, lifeTimeMin, 30.0f);
			ImGui::SliderFloat("LifeTimeMin", &lifeTimeMin, 0.0f, lifeTimeMax);
		}
		//エフェクトの最大数を写す
		int maxEffects = editParam_["MaxEffects"];
		if (ImGui::CollapsingHeader("エフェクトの最大数")) {
			ImGui::SliderInt("MaxEffects", &maxEffects, 1, 1000);
		}
		//1秒あたりのエフェクトの生成量を写す
		int emitRate = editParam_["EmitRate"];
		if (ImGui::CollapsingHeader("1秒あたりのエフェクトの生成量")) {
			ImGui::SliderInt("EmitRate", &emitRate, 1, 100);
		}
		//ブレンドモードを写す
		int blendMode = editParam_["BlendMode"];
		const char* blendModeList[] = { "None","Normal","Add","Subtract","Multiply","Screen","Exclusion" };
		if (ImGui::CollapsingHeader("ブレンドモード")) {
			ImGui::Combo("Blend Mode", &blendMode, blendModeList, (int)BlendMode::kMaxBlendModeNum);
		}
		//editParamに変更を反映
		editParam_["StartColor"]["Max"]["x"] = startColorMax.x;
		editParam_["StartColor"]["Max"]["y"] = startColorMax.y;
		editParam_["StartColor"]["Max"]["z"] = startColorMax.z;
		editParam_["StartColor"]["Max"]["w"] = startColorMax.w;
		editParam_["StartColor"]["Min"]["x"] = startColorMin.x;
		editParam_["StartColor"]["Min"]["y"] = startColorMin.y;
		editParam_["StartColor"]["Min"]["z"] = startColorMin.z;
		editParam_["StartColor"]["Min"]["w"] = startColorMin.w;
		editParam_["EndColor"]["Max"]["x"] = endColorMax.x;
		editParam_["EndColor"]["Max"]["y"] = endColorMax.y;
		editParam_["EndColor"]["Max"]["z"] = endColorMax.z;
		editParam_["EndColor"]["Max"]["w"] = endColorMax.w;
		editParam_["EndColor"]["Min"]["x"] = endColorMin.x;
		editParam_["EndColor"]["Min"]["y"] = endColorMin.y;
		editParam_["EndColor"]["Min"]["z"] = endColorMin.z;
		editParam_["EndColor"]["Min"]["w"] = endColorMin.w;
		editParam_["StartSize"]["Max"] = startSizeMax;
		editParam_["StartSize"]["Min"] = startSizeMin;
		editParam_["EndSize"]["Max"] = endSizeMax;
		editParam_["EndSize"]["Min"] = endSizeMin;
		editParam_["Velocity"]["Max"]["x"] = velocityMax.x;
		editParam_["Velocity"]["Max"]["y"] = velocityMax.y;
		editParam_["Velocity"]["Max"]["z"] = velocityMax.z;
		editParam_["Velocity"]["Min"]["x"] = velocityMin.x;
		editParam_["Velocity"]["Min"]["y"] = velocityMin.y;
		editParam_["Velocity"]["Min"]["z"] = velocityMin.z;
		editParam_["LifeTime"]["Max"] = lifeTimeMax;
		editParam_["LifeTime"]["Min"] = lifeTimeMin;
		editParam_["MaxEffects"] = maxEffects;
		editParam_["EmitRate"] = emitRate;
		editParam_["BlendMode"] = blendMode;
		//パーティクルに反映
		particle_->SetParam(editParam_);

		//セーブボタン
		if (ImGui::Button("セーブ")) {

		}
		ImGui::End();

		//エミッター

	}

#endif // _DEBUG
}

void ParticleCreatorScene::EditWithImGui() {
}

void ParticleCreatorScene::ResetWithImGui() {
#ifdef _DEBUG
	if (isReset_) {
		ImGui::OpenPopup("確認");
	}
	ImGui::SetNextWindowPos(ImVec2(510, 30));
	if (ImGui::BeginPopupModal("確認", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("リセットしますか？\n[注意] 編集中のパラメーターは反映されません\n ");
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
