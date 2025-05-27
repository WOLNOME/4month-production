#include "GamePlayScene.h"

#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "LineDrawerCommon.h"
#include "SpriteCommon.h"
#include "SceneManager.h"
#include <numbers>
#include "application/MathUtils.h"

uint32_t GamePlayScene::stageNum_ = 1;

void GamePlayScene::Initialize()
{
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

	// インターバルの数字
	textureHandleIntervalNum1_ = TextureManager::GetInstance()->LoadTexture("count1.png");
	textureHandleIntervalNum2_ = TextureManager::GetInstance()->LoadTexture("count2.png");
	textureHandleIntervalNum3_ = TextureManager::GetInstance()->LoadTexture("count3.png");
	
	spriteUI_Num1_ = std::make_unique<Sprite>();
	spriteUI_Num1_->Initialize(textureHandleIntervalNum1_);
	spriteUI_Num1_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteUI_Num1_->SetPosition({ 640.0f, 360.0f });
	spriteUI_Num1_->SetSize({ 0.0f, 0.0f });
	spriteUI_Num2_ = std::make_unique<Sprite>();
	spriteUI_Num2_->Initialize(textureHandleIntervalNum2_);
	spriteUI_Num2_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteUI_Num2_->SetPosition({ 640.0f, 360.0f });
	spriteUI_Num2_->SetSize({ 0.0f, 0.0f });
	spriteUI_Num3_ = std::make_unique<Sprite>();
	spriteUI_Num3_->Initialize(textureHandleIntervalNum3_);
	spriteUI_Num3_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteUI_Num3_->SetPosition({ 640.0f, 360.0f });
	spriteUI_Num3_->SetSize({ 0.0f, 0.0f });

	// 残り出現数UIテキスト
	remainingSpawnNumText_ = std::make_unique<TextWrite>();
	remainingSpawnNumText_->Initialize("REMAINING");
	remainingSpawnNumText_->SetParam({ 10.0f, 100.0f }, Font::UDDegitalNK_R, 40.0f, { 1, 1, 1, 1 });
	remainingSpawnNumText_->SetEdgeParam({ 0, 0, 0, 1 }, 3.0f, 0.0f, true);
	// 残りの出現数テキスト
	numText_ = std::make_unique<TextWrite>();
	numText_->Initialize("NUM");
	numText_->SetParam({ 50.0f, 150.0f }, Font::UDDegitalNK_R, 70.0f, { 0.9f, 0.85f, 0.13f, 1 });
	numText_->SetEdgeParam({ 0, 0, 0, 1 }, 3.0f, 0.0f, true);
	// 値のテキスト
	valueText_ = std::make_unique<TextWrite>();
	valueText_->Initialize("value");
	valueText_->SetParam({ 150.0f, 175.0f }, Font::UDDegitalNK_R, 40.0f, { 1, 1, 1, 1 });
	valueText_->SetEdgeParam({ 0, 0, 0, 1 }, 3.0f, 0.0f, true);
	

	switch (stageNum_)
	{
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
	switch (stageNum_)
	{
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
	for (uint32_t i = 0; i < 1; ++i)
	{
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
	enemyManager_->Initialize(camera_.get(), &players_, "enemy", "Fan","Freeze");

	switch (stageNum_)
	{
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
	switch (stageNum_)
	{
	case 1: case 2:
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

	switch (stageNum_)
	{
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

	for (size_t i = 0; i < obstaclePositions.size(); ++i)
	{
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
	switch (stageNum_)
	{
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

	switch (stageNum_)
	{
	case 5:
		for (size_t i = 0; i < bumperPositions.size(); ++i)
		{
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
		for (size_t i = 0; i < bumperPositions.size(); ++i)
		{
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

	switch (stageNum_)
	{
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

	for (size_t i = 0; i < iceFloorPositions.size(); ++i)
	{
		std::unique_ptr<IceFloor>& iceFloor = icefloors_.emplace_back();
		iceFloor = std::make_unique<IceFloor>();
		iceFloor->Initialize();
		iceFloor->SetPosition(iceFloorPositions[i]);
		iceFloor->SetScale(iceFloorScales[i]);
		iceFloor->SetRotation(iceFloorRotations[i]);
	}

	// プレイヤースポーン位置モデル
	for (uint32_t i = 0; i < playerSpawnNum_; ++i)
	{
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
}

void GamePlayScene::Finalize()
{
	for (auto& player : players_)
	{
		player->Finalize();
	}
	

	if (preSpawnedPlayer_) {
		preSpawnedPlayer_->Finalize();
	}

	enemyManager_->Finalize();

	field_->Finalize();

	for (std::unique_ptr<Obstacle>& obstacle : obstacles_)
	{
		obstacle->Finalize();
	}
	for (std::unique_ptr<Bumper>& bumper : bumpers_)
	{
		bumper->Finalize();
	}
	for (std::unique_ptr<IceFloor>& iceFloor : icefloors_)
	{
		iceFloor->Finalize();
	}

	bgm_->Stop();
	bgm_.reset();
}

void GamePlayScene::Update()
{
	//ポーズシステムの更新
	pauseSystem_->Update();
	if (pauseSystem_->GetIsPause()) {
		return;
	}
	// スプライト
	spriteUI_PLAY_->Update();
	spriteUI_Charge_->Update();
	spriteUI_ChargeGage_->Update();
	remainingSpawnNum();
	spriteUI_Num1_->Update();
	spriteUI_Num2_->Update();
	spriteUI_Num3_->Update();

	// ゲーム開始のインターバル
	if (!isGameStart_) {
		// 初期モデルだけ 1 回 Update しておく
		if (!hasPreUpdated_) {
			// ここで全プレイヤー・敵・エフェクトの Update を呼ぶ
			UpdateTransform();

			hasPreUpdated_ = true;
		}

		// 3秒カウント
		gameStartDelayTimer_ -= 1.0f / 60.0f;
		if (gameStartDelayTimer_ <= 0.0f) {
			isGameStart_ = true;
		}

		// プレイヤーの行動不能フラグセット
		for (auto& player : players_)
		{
			player->IsMoveable(false);
		}

		// エネミーの行動不能フラグセット
		enemyManager_->IsMoveable(false);
	} 
	else
	{
		for (auto& player : players_)
		{
			player->IsMoveable(true);
		}

		enemyManager_->IsMoveable(true);
	}


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
		[this](const std::unique_ptr<Player>& player)
		{
			if (player->IsDead())
			{
				player->Finalize();
				playerNum_--;
				return true;
			}
			return false;
		}), players_.end());
	// 通常プレイヤーの更新
	for (auto& player : players_)
	{
		player->Update();
	}
	if (isGameStart_)
	{
		// プレイヤー攻撃チャージ
		playerTackleCharge();
	}
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
	for (std::unique_ptr<Obstacle>& obstacle : obstacles_)
	{
		obstacle->Update();
	}
	//跳ね返る障害物
	for (std::unique_ptr<Bumper>& bumper : bumpers_)
	{
		bumper->Update();
	}
	//氷の床
	for (std::unique_ptr<IceFloor>& iceFloor : icefloors_)
	{
		iceFloor->Update();
	}

	// プレイヤースポーンのオブジェクト
	for (auto& playerSpawn : playerSpawn_)
	{
		playerSpawn->Update();
	}
	if (playerNum_ > 0 && isGameStart_)
	{
		playerSpawnRotation();
	}

	// 当たり判定
	appCollisionManager_->CheckAllCollision();

	//最も近い敵やプレイヤーの位置を計算
	CalculateNearestPosition();

	//ゲーム終了判定
	if ((playerNum_ <= 0 or enemyManager_->GetEnemyCount() == 0) and !isGameEnd_)
	{
		isGameEnd_ = true;
		isZoomIn_ = true;
		if (playerNum_ <= 0)
		{
			cameraEndPosition_ = nearestEnemyPos_;
		}
		else if (enemyManager_->GetEnemyCount() == 0)
		{
			cameraEndPosition_ = nearestPlayerPos_;
		}
	}
	// ゲームオーバーへ
	if (playerNum_ <= 0 and isGameEnd_ and !isZoomIn_ and cameraEaseTime_ >= cameraEaseDuration_ / 4.0f)
	{
		sceneManager_->SetNextScene("GAMEOVER");
	}
	// クリア
	if (input_->TriggerKey(DIK_TAB) or (enemyManager_->GetEnemyCount() == 0 and isGameEnd_ and !isZoomIn_ and cameraEaseTime_ >= cameraEaseDuration_ / 4.0f))
	{
		sceneManager_->SetNextScene("CLEAR");
	}

	//揺らす処理
	CheckShake();

	// ImGui
	ImGuiDraw();

	//UIテキスト用のImGui
	valueText_->DebugWithImGui();
}

void GamePlayScene::Draw()
{
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
	for (auto& player : players_)
	{
        player->Draw(*camera_.get());
	}

	//エネミーマネージャーの描画
	enemyManager_->Draw();

	//スカイドーム
	skydome_->Draw(*camera_.get());
	// フィールド
	field_->Draw(*camera_.get());

	//障害物
	for (std::unique_ptr<Obstacle>& obstacle : obstacles_)
	{
		obstacle->Draw(*camera_.get());
	}
	//跳ね返る障害物
	for (std::unique_ptr<Bumper>& bumper : bumpers_)
	{
		bumper->Draw(*camera_.get());
	}
	//氷の床
	for (std::unique_ptr<IceFloor>& iceFloor : icefloors_)
	{
		iceFloor->Draw(*camera_.get());
	}

	// プレイヤースポーン
	for (auto& playerSpawn : playerSpawn_)
	{
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

	if (!isGameStart_)
	{
		UpdateIntervalNum();
	}

	///------------------------------///
	///↑↑↑↑スプライト描画終了↑↑↑↑
	///------------------------------///
}

void GamePlayScene::TextDraw() {
	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///

	// ポーズシステムのテキスト描画
	pauseSystem_->TextDraw();

	if (!pauseSystem_->GetIsPause()) {
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

void GamePlayScene::ImGuiDraw()
{

#ifdef _DEBUG

	ImGui::Begin("scene");
	ImGui::Text("%s", "GAMEPLAY");;
	
	ImGui::Text("%s","ToClear : TAB");
	ImGui::Text("%s", "ToGameOver : ENTER");

	ImGui::SliderFloat3("cameraTranslate", &cameraTranslate.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("cameraRotate", &cameraRotate.x, -5.0f, 5.0f);

	ImGui::SliderFloat3("playerSpawnPos", &playerSpawnPositions_[0].x, -10.0f, 10.0f);
	// プレイヤーを追加するボタン
	if (ImGui::Button("Add Player"))
	{
		auto player = std::make_unique<Player>();

		player->SetPlayerPos(playerSpawnPositions_[0]);
		player->Initialize();
		player->SetIsChargeMax(&isChargeMax_);

		players_.push_back(std::move(player));
	}

	// 障害物の位置調整
	for (size_t i = 0; i < obstacles_.size(); ++i)
	{
		Vector3 pos = obstacles_[i]->GetPosition();
		if (ImGui::SliderFloat3(("ObstaclePos" + std::to_string(i)).c_str(), &pos.x, -10.0f, 10.0f))
		{
			obstacles_[i]->SetPosition(pos);
		}
	}
	// 跳ね返る障害物の位置と大きさの調整
	for (size_t i = 0; i < bumpers_.size(); ++i)
	{
		Vector3 pos = bumpers_[i]->GetPosition();
		Vector3 scale = bumpers_[i]->GetScale();
		if (ImGui::SliderFloat3(("BumperPos" + std::to_string(i)).c_str(), &pos.x, -20.0f, 20.0f))
		{
			bumpers_[i]->SetPosition(pos);
		}
		if (ImGui::SliderFloat3(("BumperScale" + std::to_string(i)).c_str(), &scale.x, 0.1f, 10.0f))
		{
			bumpers_[i]->SetScale(scale);
		}
	}
	// IceFloorの位置、スケール、回転の調整
	for (size_t i = 0; i < icefloors_.size(); ++i)
	{
		Vector3 pos = icefloors_[i]->GetPosition();
		Vector3 scale = icefloors_[i]->GetScale();
		Vector3 rotate = icefloors_[i]->GetRotation();
		if (ImGui::SliderFloat3(("IceFloorPos" + std::to_string(i)).c_str(), &pos.x, -20.0f, 20.0f))
		{
			icefloors_[i]->SetPosition(pos);
		}
		if (ImGui::SliderFloat3(("IceFloorScale" + std::to_string(i)).c_str(), &scale.x, 0.1f, 10.0f))
		{
			icefloors_[i]->SetScale(scale);
		}
		if (ImGui::SliderFloat3(("IceFloorRotate" + std::to_string(i)).c_str(), &rotate.x, -180.0f, 180.0f))
		{
			icefloors_[i]->SetRotation(rotate);
		}
	}

	ImGui::Text("howManyBoogie : %d", howManyBoogie_);

	ImGui::Text("charge : % .0f", charge_);

	if (!players_.empty())
	{
		bool isChargeMax = players_[0]->IsChargeMax();
		ImGui::Text("player ChargeMax : %s", isChargeMax ? "true" : "false");
	}

	ImGui::End();

	// プレイヤー
	for (auto& player : players_)
	{
		player->ImGuiDraw();
	}

	

	// フィールド
	field_->ImGuiDraw();

	// スポーン位置
	for (auto& playerSpawn : playerSpawn_)
	{
		playerSpawn->ImGuiDraw();
	}

	//ポーズシステム
	pauseSystem_->DebugWithImGui();

#endif // _DEBUG

}

void GamePlayScene::playerSpawnRotation()
{
	rotationTimer_ -= 1.0f;

	// スポーン準備：タイマーが120のとき
	if (rotationTimer_ == 120.0f && howManyBoogie_ < kMaxSpawnNum)
	{
		playerSpawn_[playerSpawnIndex_]->ParticleStart();
		playerSpawn_[playerSpawnIndex_]->IsShaking(true);

		// 小さい状態で非アクティブなプレイヤー生成
		auto prePlayer = std::make_unique<Player>();
		prePlayer->SetPosition(playerSpawnPositions_[playerSpawnIndex_]);
		prePlayer->SetScale({ 0.1f, 0.1f, 0.1f });
		prePlayer->Initialize();
		prePlayer->SetIsChargeMax(&isChargeMax_);

		prePlayer->IsMoveable(false);

		preSpawnedPlayer_ = std::move(prePlayer);

		preSpawnedPlayer_->Update();
	}

	// 拡大処理（Lerp）
	if (preSpawnedPlayer_)
	{
		float t = (120.0f - rotationTimer_) / 120.0f;
		t = std::clamp(t, 0.0f, 1.0f);

		// Lerpでスケール補間
		float lerpedScale = std::lerp(0.1f, 1.0f, t);
		Vector3 scale = { lerpedScale, lerpedScale, lerpedScale };
		preSpawnedPlayer_->SetScale(scale);
	}

	// タイマーが0以下になったらプレイヤーを有効にする
	if (rotationTimer_ <= 0.0f)
	{
		playerSpawn_[playerSpawnIndex_]->ParticleStop();
		playerSpawn_[playerSpawnIndex_]->IsShaking(false);

		if (howManyBoogie_ < kMaxSpawnNum && preSpawnedPlayer_)
		{
			rotationTimer_ = rotation_;

			preSpawnedPlayer_->SetScale({ 1.0f, 1.0f, 1.0f });
			preSpawnedPlayer_->IsMoveable(true);

			players_.push_back(std::move(preSpawnedPlayer_));
			howManyBoogie_++;
			playerNum_++;

			playerSpawnIndex_++;
			if (playerSpawnIndex_ > playerSpawnNum_ - 1)
			{
				playerSpawnIndex_ = 0;
			}
		}
	}
}

void GamePlayScene::playerTackleCharge()
{
	// プレイヤーが1体以上いるとき
	if (playerNum_ > 0)
	{
		// チャージが最大でないとき
		if (charge_ < chargeMax_)
		{
			charge_ += 1.0f;
		}
		// チャージが最大値に達したら
		else
		{
			if (!isChargeMax_) 
			{
				isChargeMax_ = true;
			}

			if (input_->TriggerKey(DIK_SPACE) && isChargeMax_) {

				for (std::unique_ptr<Player>& player : players_)
				{
					if (!player->IsAttack()) 
					{
						continue;
					}
					isChargeMax_ = false;
					charge_ = 0.0f;

					return;
				}

			}
		}

	}
}

void GamePlayScene::CheckShake() {
	//全てのプレイヤーのシェイク判定を処理
	for (auto& player : players_) {
		if (player->isDamageShake_) {
			camera_->RegistShake(0.4f, 0.15f);
			player->isDamageShake_ = false;
		}
		if (player->isDeadShake_) {
			camera_->RegistShake(0.4f, 0.4f);
			player->isDeadShake_ = false;
		}
	}
	//全てのエネミーのシェイク判定を処理
	for (auto& enemy : enemyManager_->GetAllEnemies()) {
		if (enemy->isDamageShake_) {
			camera_->RegistShake(0.4f, 0.25f);
			enemy->isDamageShake_ = false;
		}
		if (enemy->isDeadShake_) {
			camera_->RegistShake(0.4f, 0.5f);
			enemy->isDeadShake_ = false;
		}
	}
}

void GamePlayScene::remainingSpawnNum()
{
	// 残り出現数を計算
	remainingBoogie_ = kMaxSpawnNum - howManyBoogie_;
	if (remainingBoogie_ < 0) {
		remainingBoogie_ = 0; // 負の値にならないように制限
	}
}

void GamePlayScene::UpdateTransform()
{
	// プレイヤーの位置を更新
	for (auto& player : players_)
	{
		player->UpdateModel();
	}
	// エネミーマネージャーの位置を更新
	enemyManager_->UpdateTransform();
	// フィールドの位置を更新
	field_->UpdateTransform();
	// 障害物の位置を更新
	for (std::unique_ptr<Obstacle>& obstacle : obstacles_)
	{
		obstacle->UpdateTransform();
	}
	// 跳ね返る障害物の位置を更新
	for (std::unique_ptr<Bumper>& bumper : bumpers_)
	{
		bumper->UpdateTransform();
	}
	// 氷の床の位置を更新
	for (std::unique_ptr<IceFloor>& iceFloor : icefloors_)
	{
		iceFloor->UpdateTransform();
	}
}

void GamePlayScene::UpdateIntervalNum()
{
	if (gameStartDelayTimer_ <= 3.0f && gameStartDelayTimer_ > 2.0f){
		spriteUI_Num3_->Draw();
	} 
	else if (gameStartDelayTimer_ <= 2.0f && gameStartDelayTimer_ > 1.0f){
		spriteUI_Num2_->Draw();
	} 
	else if (gameStartDelayTimer_ <= 1.0f && gameStartDelayTimer_ > 0.0f){
		spriteUI_Num1_->Draw();
	} 

	// 数字のサイズを更新
	spriteUI_Num1_->SetSize(numSize_);
	spriteUI_Num2_->SetSize(numSize_);
	spriteUI_Num3_->SetSize(numSize_);

	numSizeTimer_++;
	if (numSizeTimer_ > 60) {
		numSizeTimer_ = 0; // リピート or 一度きりなら終了
	}

	// 0〜π までの範囲に変換（0→最大→0）
	float t = static_cast<float>(numSizeTimer_) / 60.0f; // 0〜1
	float scale = std::sinf(t * 3.14159265f); // sin(0→π) = 0→1→0

	// 最大サイズに合わせてスケーリング
	numSize_.x = 320.0f * scale;
	numSize_.y = 480.0f * scale;
}

void GamePlayScene::UpdateCamera()
{
	//　カメラの演出
	if (isZoomIn_)
	{
		UpdateZoomIn();
	}
	else if (waitTime_ > 0.0f)
	{
		waitTime_ -= 1.0f / 60.0f;
		if (waitTime_ <= 0.0f)
		{
			isZoomOut_ = true;
		}
	}
	else if (isZoomOut_)
	{
		UpdateZoomOut();
	}
	// カメラの行列を更新
	camera_->UpdateMatrix();
}

void GamePlayScene::UpdateZoomIn()
{
	// 時間を経過させる
	cameraEaseTime_ += 1.0f / 60.0f;
	float t = cameraEaseTime_ / cameraEaseDuration_;
	// 補完が1.0fを超えないようにする
	if (t > 1.0f) t = 1.0f;

	// 補完のイージング
	float easeT = EaseInOutQuad(t);
	//カメラのスタート地点とエンド地点を設定
	Vector3 cameraStartPosition = cameraStartPosition_;
	Vector3 cameraEndPosition = cameraEndPosition_;
	cameraEndPosition.y = 2.0f;
	cameraEndPosition.z -= 14.0f;
	// カメラの補間
	Vector3 newPosition = cameraStartPosition_ * (1.0f - easeT) + cameraEndPosition * easeT;
	Vector3 newRotation = cameraRotate * (1.0f - easeT) + CalculateLookAtRotation(newPosition, cameraEndPosition_) * easeT; // カメラの向きを補間
	newRotation -= 0.2f;

	// カメラに新しい位置と回転を設定
	camera_->SetTranslate(newPosition);
	camera_->SetRotate(newRotation);

	//補間が終わったら
	if (cameraEaseTime_ >= 3.0f)
	{
		isZoomIn_ = false;
		cameraEaseTime_ = 0.0f;
		waitTime_ = waitTimeDuration_;
	}
}

void GamePlayScene::UpdateZoomOut()
{
	// 時間を経過させる
	cameraEaseTime_ += 1.0f / 60.0f;
	float t = cameraEaseTime_ / cameraEaseDuration_;
	// 補完が1.0fを超えないようにする
	if (t > 1.0f) t = 1.0f;

	// 補完のイージング
	float easeT = EaseInOutQuad(t);
	Vector3 cameraEndPosition = cameraEndPosition_;
	cameraEndPosition.y = 2.0f;
	cameraEndPosition.z -= 14.0;
	// カメラの補間
	Vector3 newPosition = cameraEndPosition * (1.0f - easeT) + cameraStartPosition_ * easeT;
	Vector3 newRotation = CalculateLookAtRotation(newPosition, cameraEndPosition_) * (1.0f - easeT) + cameraRotate * easeT;
	Vector3 cameraRotate = { 0.95f, 0.0f, 0.0f };
	newRotation = newRotation * (1.0f - easeT) + cameraRotate * easeT;

	// カメラに新しい位置と回転を設定
	camera_->SetTranslate(newPosition);
	camera_->SetRotate(newRotation);

	//補間が終わったら
	if (t >= 1.0f)
	{
		isZoomOut_ = false;
		cameraEaseTime_ = 0.0f;
	}
}

void GamePlayScene::CalculateNearestPosition()
{
	// プレイヤーが残り1人のとき
	if (players_.size() == 1)
	{
		//最も近い敵の座標を取得
		float closestDistance_ = (std::numeric_limits<float>::max)();

		//最も近い敵の座標を取得
		//タックルエネミー
		for (uint32_t i = 0; i < enemyManager_->GetTackleEnemyCount(); i++)
		{
			//地面にいないエネミーは無視
			if (!enemyManager_->GetTackleEnemy(i)->IsGround()) { continue; }
			Vector3 enemyPos = enemyManager_->GetTackleEnemyPosition(i);
			float distance = (players_[0]->GetPosition() - enemyPos).Length();
			if (distance < closestDistance_)
			{
				closestDistance_ = distance;
				nearestEnemyPos_ = enemyPos;
				nearestEnemyType_ = "TackleEnemy";
				nearestEnemyNum_ = i;
			}
		}
		//ファンエネミー
		for (uint32_t i = 0; i < enemyManager_->GetFanEnemyCount(); i++)
		{
			//地面にいないエネミーは無視
			if (!enemyManager_->GetFanEnemy(i)->IsGround()) { continue; }
			Vector3 enemyPos = enemyManager_->GetFanEnemyPosition(i);
			float distance = (players_[0]->GetPosition() - enemyPos).Length();
			if (distance < closestDistance_)
			{
				closestDistance_ = distance;
				nearestEnemyPos_ = enemyPos;
				nearestEnemyType_ = "FanEnemy";
				nearestEnemyNum_ = i;
			}
		}
		//フリーズエネミー
		for (uint32_t i = 0; i < enemyManager_->GetFreezeEnemyCount(); i++)
		{
			if (!enemyManager_->GetFreezeEnemy(i)->IsGround()) { continue; }
			Vector3 enemyPos = enemyManager_->GetFreezeEnemyPosition(i);
			float distance = (players_[0]->GetPosition() - enemyPos).Length();
			if (distance < closestDistance_)
			{
				closestDistance_ = distance;
				nearestEnemyPos_ = enemyPos;
				nearestEnemyType_ = "FreezeEnemy";
			}
		}
		if (closestDistance_ == (std::numeric_limits<float>::max)())
		{
			//敵がみんな地面にいない時はフィールドの中心を向く
			nearestEnemyPos_ = { 0.0f, 1.0f, 0.0f };
			nearestEnemyType_ = "";
		}
	}

	// エネミーが残り1体のとき
	if (enemyManager_->GetEnemyCount() == 1)
	{
		//最も近いプレイヤーの座標を取得
		float closestDistance_ = (std::numeric_limits<float>::max)();
		//最も近いプレイヤーの座標を取得
		for (uint32_t i = 0; i < playerNum_; i++)
		{
			//地面にいないプレイヤーは無視
			if (!players_[i]->IsGround()) { continue; }
			Vector3 playerPos = players_[i]->GetPosition();
			float distance;
			//タックルエネミー
			for (uint32_t j = 0; j < enemyManager_->GetTackleEnemyCount(); j++)
			{
				Vector3 enemyPos = enemyManager_->GetTackleEnemyPosition(j);
				distance = (playerPos - enemyPos).Length();
				if (distance < closestDistance_)
				{
					closestDistance_ = distance;
					nearestPlayerPos_ = playerPos;
					nearestPlayerNum_ = i;
				}
			}
			//ファンエネミー
			for (uint32_t j = 0; j < enemyManager_->GetFanEnemyCount(); j++)
			{
				Vector3 enemyPos = enemyManager_->GetFanEnemyPosition(j);
				distance = (playerPos - enemyPos).Length();
				if (distance < closestDistance_)
				{
					closestDistance_ = distance;
					nearestPlayerPos_ = playerPos;
					nearestPlayerNum_ = i;
				}
			}
			//フリーズエネミー
			for (uint32_t j = 0; j < enemyManager_->GetFreezeEnemyCount(); j++)
			{
				Vector3 enemyPos = enemyManager_->GetFreezeEnemyPosition(j);
				distance = (playerPos - enemyPos).Length();
				if (distance < closestDistance_)
				{
					closestDistance_ = distance;
					nearestPlayerPos_ = playerPos;
					nearestPlayerNum_ = i;
				}
			}
		}
		if (closestDistance_ == (std::numeric_limits<float>::max)())
		{
			//プレイヤーがみんな地面にいない時はフィールドの中心を向く
			nearestPlayerPos_ = { 0.0f, 1.0f, 0.0f };
			nearestPlayerNum_ = (std::numeric_limits<uint32_t>::max)();
		}

	}

}