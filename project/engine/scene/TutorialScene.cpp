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
