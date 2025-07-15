#include "TutorialScene.h"

#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "LineDrawerCommon.h"
#include "SpriteCommon.h"
#include "SceneManager.h"
#include <numbers>
#include "application/MathUtils.h"

#include "../../application/UI/RemainingSpawnNum.h"

void TutorialScene::Initialize() {
	//シーンの初期化
	GamePlayScene::Initialize();

	isGameStart_ = true; // ゲーム開始フラグをtrueに設定

	//チュートリアルシステム
	tutorialSystem_ = std::make_unique<TutorialSystem>();
	tutorialSystem_->Initialize();
}

void TutorialScene::Update() {
	
	// チュートリアルシステムの更新
	tutorialSystem_->Update();
	if (tutorialSystem_->GetIsTimeStop()) {
		return;
	}

	GamePlayScene::Update();

}

void TutorialScene::Draw() {
	
	GamePlayScene::Draw();

	if (!pauseSystem_->GetIsPause()) {
		// チュートリアルシステムの描画
		tutorialSystem_->DrawSprite();
	}

	
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
		remainingSpawnNum_->TextDraw();
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
	stageSpawnPositions_.push_back({ 0.0f,1.0f,5.0f });
	stageSpawnPositions_.push_back({ 5.0f,1.0f,-5.0f });
	stageSpawnPositions_.push_back({ -5.0f,1.0f,-5.0f });

	playerManager_->SetSpawnPositions(stageSpawnPositions_);
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

void TutorialScene::UpdateIntervalNum()
{
	// チュートリアルでは間隔の数字を更新しない
	// ここは空の実装
}

void TutorialScene::StartInterVal()
{
	// チュートリアルでは間隔の数字を開始しない
	// ここは空の実装
}
