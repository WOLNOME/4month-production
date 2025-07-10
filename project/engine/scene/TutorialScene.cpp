#include "TutorialScene.h"

#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "LineDrawerCommon.h"
#include "SpriteCommon.h"
#include "SceneManager.h"
#include <numbers>
#include "application/MathUtils.h"

void TutorialScene::Initialize() {
	//シーンの初期化
	GamePlayScene::Initialize();

	//チュートリアルシステム
	tutorialSystem_ = std::make_unique<TutorialSystem>();
	tutorialSystem_->Initialize();
}

void TutorialScene::Update() {
	//ポーズシステムの更新
	pauseSystem_->Update();
	if (pauseSystem_->GetIsPause()) {
		return;
	}
	// チュートリアルシステムの更新
	tutorialSystem_->Update();
	if (tutorialSystem_->GetIsTimeStop()) {
		return;
	}

	// スプライト
	spriteUI_PLAY_->Update();
	spriteUI_Charge_->Update();
	spriteUI_ChargeGage_->Update();
	remainingSpawnNum();

	// チャージの大きさに応じてスプライトのサイズを変更
	// チャージの割合を計算
	float chargeScale = charge_ / chargeMax_;
	chargeScale = std::clamp(chargeScale, 0.0f, 1.0f); // 0.0f～1.0fの範囲に制限

	// チャージの割合に応じてテクスチャのXサイズを変更
	float newWidth = 180.0f * chargeScale; // 0から120まで拡大
	spriteUI_Charge_->SetSize({ newWidth, 50.0f }); // 横方向のサイズを変更

	// カメラの更新
	UpdateCamera();

	// 死んだプレイヤーを削除
	players_.erase(std::remove_if(players_.begin(), players_.end(),
		[this](const std::unique_ptr<Player>& player) {
			if (player->IsDead()) {
				player->Finalize();
				playerNum_--;
				return true;
			}
			return false;
		}), players_.end());
	// 通常プレイヤーの更新
	for (auto& player : players_) {
		player->Update();
	}
	// プレイヤー攻撃チャージ
	playerTackleCharge();

	// 準備中プレイヤーの更新
	if (preSpawnedPlayer_) {
		preSpawnedPlayer_->Update();
	}


	//エネミーマネージャーの更新
	enemyManager_->Update();

	//スカイドーム
	skydome_->Update();
	// フィールド
	field_->Update();

	//障害物
	for (std::unique_ptr<Obstacle>& obstacle : obstacles_) {
		obstacle->Update();
	}
	//跳ね返る障害物
	for (std::unique_ptr<Bumper>& bumper : bumpers_) {
		bumper->Update();
	}
	//氷の床
	for (std::unique_ptr<IceFloor>& iceFloor : icefloors_) {
		iceFloor->Update();
	}

	// プレイヤースポーンのオブジェクト
	for (auto& playerSpawn : playerSpawn_) {
		playerSpawn->Update();
	}
	if (playerNum_ > 0) {
		playerSpawnRotation();
	}

	// 当たり判定
	appCollisionManager_->CheckAllCollision();

	//最も近い敵やプレイヤーの位置を計算
	CalculateNearestPosition();

	//ゲーム終了判定
	if ((playerNum_ <= 0 or enemyManager_->GetEnemyCount() == 0) and !isGameEnd_) {
		isGameEnd_ = true;
		isZoomIn_ = true;
		if (playerNum_ <= 0) {
			cameraEndPosition_ = nearestEnemyPos_;
		}
		else if (enemyManager_->GetEnemyCount() == 0) {
			cameraEndPosition_ = nearestPlayerPos_;
		}
	}
	// ゲームオーバーへ
	if (playerNum_ <= 0 and isGameEnd_ and !isZoomIn_ and cameraEaseTime_ >= cameraEaseDuration_ / 4.0f) {
		sceneManager_->SetNextScene("GAMEOVER");
	}
	// クリア
	if ((enemyManager_->GetEnemyCount() == 0 and isGameEnd_ and !isZoomIn_ and cameraEaseTime_ >= cameraEaseDuration_ / 4.0f)) {
		sceneManager_->SetNextScene("CLEAR");
	}

	//揺らす処理
	CheckShake();

	// ImGui
	ImGuiDraw();

	//UIテキスト用のImGui
	//remainingSpawnNumText_->DebugWithImGui();
	//numText_->DebugWithImGui();
	//valueText_->DebugWithImGui();
}

void TutorialScene::Draw() {
	//3Dモデルの共通描画設定
	Object3dCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓モデル描画開始↓↓↓↓
	///------------------------------///

	 // 準備中プレイヤーの描画
	if (preSpawnedPlayer_) {
		preSpawnedPlayer_->Draw(*camera_.get());
	}

	// 通常プレイヤーの描画
	for (auto& player : players_) {
		player->Draw(*camera_.get());
	}

	//エネミーマネージャーの描画
	enemyManager_->Draw();

	//スカイドーム
	skydome_->Draw(*camera_.get());
	// フィールド
	field_->Draw(*camera_.get());

	//障害物
	for (std::unique_ptr<Obstacle>& obstacle : obstacles_) {
		obstacle->Draw(*camera_.get());
	}
	//跳ね返る障害物
	for (std::unique_ptr<Bumper>& bumper : bumpers_) {
		bumper->Draw(*camera_.get());
	}
	//氷の床
	for (std::unique_ptr<IceFloor>& iceFloor : icefloors_) {
		iceFloor->Draw(*camera_.get());
	}

	// プレイヤースポーン
	for (auto& playerSpawn : playerSpawn_) {
		playerSpawn->Draw(*camera_.get());
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

	spriteUI_PLAY_->Draw();
	spriteUI_ChargeGage_->Draw();
	spriteUI_Charge_->Draw();
	//ポーズシステムの描画
	pauseSystem_->DrawSprite();
	if (!pauseSystem_->GetIsPause()) {
		// チュートリアルシステムの描画
		tutorialSystem_->DrawSprite();
	}

	///------------------------------///
	///↑↑↑↑スプライト描画終了↑↑↑↑
	///------------------------------///
}

void TutorialScene::TextDraw() {
	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///

	// ポーズシステムのテキスト描画
	pauseSystem_->TextDraw();
	if (!pauseSystem_->GetIsPause()) {
		// チュートリアルシステムのテキスト描画
		tutorialSystem_->WriteText();
	}
	if (!pauseSystem_->GetIsPause() && tutorialSystem_->GetIsZankiDisplay()) {
		//スペースUIテキスト
		remainingSpawnNumText_->WriteText(L"残り出現数");
		// 残りの出現数テキスト
		numText_->WriteText(std::to_wstring(remainingBoogie_));
		// 値のテキスト
		valueText_->WriteText(L"体");
	}

	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///
}

void TutorialScene::SetupCamera()
{
	cameraTranslate = { 0.0f,70.0f,-50.0f };

}

void TutorialScene::SetupPlayerSpawnPositions()
{
	playerSpawnPositions_.push_back({ 0.0f,1.0f,5.0f });
	playerSpawnPositions_.push_back({ 5.0f,1.0f,-5.0f });
	playerSpawnPositions_.push_back({ -5.0f,1.0f,-5.0f });
}

void TutorialScene::SetupEnemyManager()
{
	enemyManager_->SpawnTackleEnemy(2);
}

void TutorialScene::SetupField()
{
	field_->SetScale({ 20.0f,1.0f,20.0f });
	enemyManager_->SetSpawnPosition({ -20.0f,1.0f,-20.0f }, { 20.0f,1.0f,20.0f });
}

void TutorialScene::CreateObstacles()
{
	//チュートリアルでは障害物を設置しない
}

void TutorialScene::CreateBumpers()
{
	//チュートリアルでは跳ねる障害物を設置しない
}

void TutorialScene::CreateIceFloors()
{
	//チュートリアルでは氷の床を設置しない
}
