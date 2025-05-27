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
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

	// スプライト
	// プレイ画面UI
	textureHandleUI_PLAY_ = TextureManager::GetInstance()->LoadTexture("UI_PLAY.png");
	spriteUI_PLAY_ = std::make_unique<Sprite>();
	spriteUI_PLAY_->Initialize(textureHandleUI_PLAY_);

	// チャージUI
	textureHandleUI_Charge_ = TextureManager::GetInstance()->LoadTexture("spawn.png");
	spriteUI_Charge_ = std::make_unique<Sprite>();
	spriteUI_Charge_->SetPosition({ 1085.0f, 600.0f });
	spriteUI_Charge_->Initialize(textureHandleUI_Charge_);

	spriteUI_ChargeGage_ = std::make_unique<Sprite>();
	spriteUI_ChargeGage_->Initialize(textureHandleUI_Charge_);
	spriteUI_ChargeGage_->SetPosition({ 1085.0f, 600.0f });
	spriteUI_ChargeGage_->SetSize({ 180.0f, 50.0f });
	spriteUI_ChargeGage_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	// 残り出現数UIテキスト
	remainingSpawnNumText_ = std::make_unique<TextWrite>();
	remainingSpawnNumText_->Initialize("REMAINING");
	remainingSpawnNumText_->SetParam({ 10.0f, 100.0f }, Font::GenEiPOPle, 40.0f, { 1, 1, 1, 1 });
	remainingSpawnNumText_->SetEdgeParam({ 0, 0, 0, 1 }, 3.0f, 0.0f, true);
	// 残りの出現数テキスト
	numText_ = std::make_unique<TextWrite>();
	numText_->Initialize("NUM");
	numText_->SetParam({ 50.0f, 150.0f }, Font::GenEiPOPle, 70.0f, { 0.9f, 0.85f, 0.13f, 1 });
	numText_->SetEdgeParam({ 0, 0, 0, 1 }, 3.0f, 0.0f, true);
	// 値のテキスト
	valueText_ = std::make_unique<TextWrite>();
	valueText_->Initialize("value");
	valueText_->SetParam({ 150.0f, 175.0f }, Font::GenEiPOPle, 40.0f, { 1, 1, 1, 1 });
	valueText_->SetEdgeParam({ 0, 0, 0, 1 }, 3.0f, 0.0f, true);


	switch (stageNum_) {
	case 1: case 2:
		cameraTranslate = { 0.0f,70.0f,-50.0f };

		break;

	case 3: case 4: case 5:
		cameraTranslate = { 0.0f,100.0f,-75.0f };

		break;
	default:
		break;
	}

	//カメラの生成と初期化
	camera_ = std::make_unique<BaseCamera>();
	camera_->Initialize();
	camera_->SetRotate({ cameraRotate });
	camera_->SetTranslate(cameraTranslate);
	camera_->SetFarClip(200.0f);
	//パーティクルマネージャーにカメラをセット
	ParticleManager::GetInstance()->SetCamera(camera_.get());

	// 当たり判定
	appCollisionManager_ = AppCollisionManager::GetInstance();
	appCollisionManager_->Initialize();

	// スポーン位置
	switch (stageNum_) {
	case 0:
		playerSpawnPositions_.push_back({ 0.0f,1.0f,5.0f });
		playerSpawnPositions_.push_back({ 5.0f,1.0f,-5.0f });
		playerSpawnPositions_.push_back({ -5.0f,1.0f,-5.0f });
		break;

	case 1:
		playerSpawnPositions_.push_back({ 0.0f,1.0f,5.0f });
		playerSpawnPositions_.push_back({ 5.0f,1.0f,-5.0f });
		playerSpawnPositions_.push_back({ -5.0f,1.0f,-5.0f });
		break;

	case 2:
		playerSpawnPositions_.push_back({ 5.0f,1.0f,5.0f });
		playerSpawnPositions_.push_back({ -5.0f,1.0f,5.0f });
		playerSpawnPositions_.push_back({ 0.0f,1.0f,-5.0f });
		break;

	case 3:
		playerSpawnPositions_.push_back({ 0.0f,1.0f,0.0f });
		playerSpawnPositions_.push_back({ 2.0f,1.0f,2.0f });
		playerSpawnPositions_.push_back({ -2.0f,1.0f,-2.0f });
		break;

	case 4:
		playerSpawnPositions_.push_back({ 0.0f,1.0f,13.0f });
		playerSpawnPositions_.push_back({ 8.0f,1.0f,-10.0f });
		playerSpawnPositions_.push_back({ -10.0f,1.0f,-10.0f });
		break;

	case 5:
		playerSpawnPositions_.push_back({ 0.0f,1.0f,0.0f });
		playerSpawnPositions_.push_back({ 2.0f,1.0f,2.0f });
		playerSpawnPositions_.push_back({ -2.0f,1.0f,-2.0f });
		break;

	default:
		break;
	}


	// プレイヤー
	for (uint32_t i = 0; i < 1; ++i) {
		auto player = std::make_unique<Player>();

		player->SetPlayerPos(playerSpawnPositions_[0]);
		player->SetScale({ 1.0f, 1.0f, 1.0f });
		player->Initialize();
		player->SetIsChargeMax(&isChargeMax_);

		players_.push_back(std::move(player));
		playerNum_++;
	}

	//エネミーマネージャーの生成と初期化
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(camera_.get(), &players_, "enemy", "Fan", "Freeze");

	switch (stageNum_) {
	case 0:
		enemyManager_->SpawnTackleEnemy(2);
		break;

	case 1:
		enemyManager_->SpawnTackleEnemy(7);
		break;

	case 2:
		enemyManager_->SpawnTackleEnemy(4);
		enemyManager_->SpawnFanEnemy(3);
		break;

	case 3:
		enemyManager_->SpawnTackleEnemy(7);
		enemyManager_->SpawnFreezeEnemy(4);
		break;

	case 4:
		enemyManager_->SpawnTackleEnemy(7);
		enemyManager_->SpawnFanEnemy(5);
		break;

	case 5:
		enemyManager_->SpawnFanEnemy(7);
		enemyManager_->SpawnFreezeEnemy(7);
		break;

	default:
		break;
	}


	//スカイドーム
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();
	// フィールド
	field_ = std::make_unique<Field>();
	field_->Initialize();

	//フィールドの大きさと敵のスポーン範囲を設定
	switch (stageNum_) {
	case 0:case 1: case 2:
		field_->SetScale({ 20.0f,1.0f,20.0f });
		enemyManager_->SetSpawnPosition({ -20.0f,1.0f,-20.0f }, { 20.0f,1.0f,20.0f });

		break;
	case 3: case 4: case 5:
		field_->SetScale({ 30.0f, 1.0f, 30.0f });
		enemyManager_->SetSpawnPosition({ -30.0f,1.0f,-30.0f }, { 30.0f,1.0f,30.0f });

		break;
	default:
		break;
	}


	//障害物の生成
	std::vector<Vector3> obstaclePositions;
	std::vector<Vector3> obstacleScales;

	switch (stageNum_) {
	case 2:
		//障害物の生成
		obstaclePositions = {
			{ 0.0f, 1.0f, 7.0f },
			{ -15.0f, 1.0f, 0.0f },
			{ 10.0f, 1.0f, -15.0f }
		};
		obstacleScales = {
			{ 5.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 5.0f },
			{ 1.0f, 1.0f, 5.0f }
		};
		break;
	default:
		break;
	}

	for (size_t i = 0; i < obstaclePositions.size(); ++i) {
		std::unique_ptr<Obstacle>& obstacle = obstacles_.emplace_back();
		obstacle = std::make_unique<Obstacle>();
		obstacle->Initialize();
		obstacle->SetPosition(obstaclePositions[i]);
		obstacle->SetScale(obstacleScales[i]);
	}

	//跳ね返る障害物の生成
	std::vector<Vector3> bumperPositions;
	std::vector<Vector3> bumperScales;
	std::vector<Vector3> bumperDirections;
	std::vector<float> bumperSpeeds;
	std::vector<float> bumperRanges;
	switch (stageNum_) {
	case 4:
		bumperPositions = {
		{ 0.0f, 1.0f, 12.0f },
		{ -20.0f, 1.0f, 0.0f },
		{ 17.0f, 1.0f, -22.0f }
		};

		bumperScales = {
			{ 3.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 3.0f },
			{ 1.0f, 1.0f, 3.0f }
		};
		break;

	case 5:

		bumperPositions = {
		{ 0.0f, 1.0f, 12.0f },
		{ -20.0f, 1.0f, 0.0f },
		{ 17.0f, 1.0f, -22.0f }
		};
		bumperScales = {
			{ 3.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 3.0f },
			{ 1.0f, 1.0f, 3.0f }
		};
		bumperDirections = {
		  { 1.0f, 0.0f, 0.0f },
		  { 0.0f, 0.0f, 1.0f },
		  { -1.0f, 0.0f, 0.0f }
		};
		bumperSpeeds = {
			0.1f,
			0.1f,
			0.1f
		};
		bumperRanges = {
			10.0f,
			10.0f,
			10.0f
		};
		break;
	default:
		break;
	}

	switch (stageNum_) {
	case 5:
		for (size_t i = 0; i < bumperPositions.size(); ++i) {
			std::unique_ptr<Bumper>& bumper = bumpers_.emplace_back();
			bumper = std::make_unique<Bumper>();
			bumper->Initialize();
			bumper->SetPosition(bumperPositions[i]);
			bumper->SetScale(bumperScales[i]);
			bumper->SetMoveDirection(bumperDirections[i]);
			bumper->SetMoveSpeed(bumperSpeeds[i]);
			bumper->SetMoveRange(bumperRanges[i]);
		}
		break;
	default:
		for (size_t i = 0; i < bumperPositions.size(); ++i) {
			std::unique_ptr<Bumper>& bumper = bumpers_.emplace_back();
			bumper = std::make_unique<Bumper>();
			bumper->Initialize();
			bumper->SetPosition(bumperPositions[i]);
			bumper->SetScale(bumperScales[i]);
		}
		break;
	}


	//氷の床の生成
	std::vector<Vector3> iceFloorPositions;
	std::vector<Vector3> iceFloorScales;
	std::vector<Vector3> iceFloorRotations;

	switch (stageNum_) {
	case 3:
		iceFloorPositions = {
		{ -20.0f, 1.01f, 15.0f },
		{ -5.0f, 1.01f, -15.0f },
		{ 20.0f, 1.01f, 0.0f }
		};

		iceFloorScales = {
			{ 5.0f, 1.0f, 5.0f },
			{ 5.0f, 1.0f, 5.0f },
			{ 5.0f, 1.0f, 5.0f }
		};

		iceFloorRotations = {
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, 45.0f, 0.0f },
			{ 0.0f, 67.0f, 0.0f }
		};
		break;

	case 5:
		iceFloorPositions = {
		{ -20.0f, 1.01f, 15.0f },
		{ -5.0f, 1.01f, -15.0f },
		{ 20.0f, 1.01f, 0.0f }
		};
		iceFloorScales = {
			{ 5.0f, 1.0f, 5.0f },
			{ 5.0f, 1.0f, 5.0f },
			{ 5.0f, 1.0f, 5.0f }
		};
		iceFloorRotations = {
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, 45.0f, 0.0f },
			{ 0.0f, 67.0f, 0.0f }
		};
		break;

	default:
		break;
	}

	for (size_t i = 0; i < iceFloorPositions.size(); ++i) {
		std::unique_ptr<IceFloor>& iceFloor = icefloors_.emplace_back();
		iceFloor = std::make_unique<IceFloor>();
		iceFloor->Initialize();
		iceFloor->SetPosition(iceFloorPositions[i]);
		iceFloor->SetScale(iceFloorScales[i]);
		iceFloor->SetRotation(iceFloorRotations[i]);
	}

	// プレイヤースポーン位置モデル
	for (uint32_t i = 0; i < playerSpawnNum_; ++i) {
		auto playerSpawn = std::make_unique<SpawnPos>();
		playerSpawn->SetPosition(playerSpawnPositions_[i]);
		playerSpawn->Initialize();

		playerSpawn_.push_back(std::move(playerSpawn));
	}

	//bgm
	bgm_ = std::make_unique<Audio>();
	bgm_->Initialize("game/bgm.wav");
	bgm_->Play(true, 0.15f);

	//ポーズシステム
	pauseSystem_ = std::make_unique<PauseSystem>();
	pauseSystem_->Initialize();

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
	if (charge_ == 0.0f) {
		// チャージが0のとき、テクスチャのXサイズを120に設定
		spriteUI_Charge_->SetSize({ 180.0f, 50.0f });
	}
	else {
		// チャージの割合を計算
		float chargeScale = charge_ / chargeMax_;
		chargeScale = std::clamp(chargeScale, 0.0f, 1.0f); // 0.0f～1.0fの範囲に制限

		// チャージの割合に応じてテクスチャのXサイズを変更
		float newWidth = 180.0f * chargeScale; // 0から120まで拡大
		spriteUI_Charge_->SetSize({ newWidth, 50.0f }); // 横方向のサイズを変更
	}


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
