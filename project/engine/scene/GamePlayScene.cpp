#include "GamePlayScene.h"

#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "ParticleCommon.h"
#include "LineDrawerCommon.h"
#include "SpriteCommon.h"
#include "SceneManager.h"
#include <numbers>

void GamePlayScene::Initialize()
{
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

	//カメラの生成と初期化
	camera_ = std::make_unique<BaseCamera>();
	camera_->Initialize();
	camera_->SetRotate({ cameraRotate });
	camera_->SetTranslate(cameraTranslate);
	camera_->SetFarClip(100.0f);

	// 当たり判定
	appCollisionManager_ = AppCollisionManager::GetInstance();
	appCollisionManager_->Initialize();

	// プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->SetCamera(camera_.get());

	// エネミー プレイヤーの位置をセットするためプレイヤーの初期化の後に行う
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();
	enemy_->SetCamera(camera_.get());
	enemy_->SetPlayerPos(player_->GetPosition());

	// フィールド
	field_ = std::make_unique<Field>();
	field_->Initialize();
	field_->SetCamera(camera_.get());

}

void GamePlayScene::Finalize()
{
	player_->Finalize();
	enemy_->Finalize();
	field_->Finalize();
}

void GamePlayScene::Update()
{
	// カメラの更新
	camera_->UpdateMatrix();
	camera_->SetRotate({ cameraRotate });
	camera_->SetTranslate(cameraTranslate);

	// プレイヤー
	player_->Update();

	// エネミー
	enemy_->SetPlayerPos(player_->GetPosition());
	enemy_->Update();

	// フィールド
	field_->Update();

	// 当たり判定
	appCollisionManager_->CheckAllCollision();


	// ImGui
	ImGuiDraw();
}

void GamePlayScene::Draw()
{
	//3Dモデルの共通描画設定
	Object3dCommon::GetInstance()->SettingCommonDrawing();
	
	///------------------------------///
	///↓↓↓↓モデル描画開始↓↓↓↓
	///------------------------------///

	// プレイヤー
	player_->Draw();

	// エネミー
	enemy_->Draw();

	// フィールド
	field_->Draw();


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

void GamePlayScene::ImGuiDraw()
{

#ifdef _DEBUG

	ImGui::Begin("scene");
	
	ImGui::SliderFloat3("cameraTranslate", &cameraTranslate.x, -50.0f, 50.0f);
	ImGui::SliderFloat3("cameraRotate", &cameraRotate.x, -5.0f, 5.0f);

	ImGui::End();

	// プレイヤー
	player_->ImGuiDraw();

	// エネミー
	enemy_->ImGuiDraw();

	// フィールド
	field_->ImGuiDraw();



#endif // _DEBUG

}
