#include "GamePlayScene.h"

#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "LineDrawerCommon.h"
#include "SpriteCommon.h"
#include "SceneManager.h"
#include <numbers>
#include "application/MathUtils.h"

#include "../../application/UI/RemainingSpawnNum.h"

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

	charge_ = std::make_unique<Charge>();
	charge_->Initialize(TextureManager::GetInstance()->LoadTexture("spawn.png"));

	// インターバルの数字
	textureHandlesIntervalNum_ = {
		TextureManager::GetInstance()->LoadTexture("count1.png"),
		TextureManager::GetInstance()->LoadTexture("count2.png"),
		TextureManager::GetInstance()->LoadTexture("count3.png")
	};

	spriteUI_Num_.resize(3);
	for (size_t i = 0; i < spriteUI_Num_.size(); ++i) {
		spriteUI_Num_[i] = std::make_unique<Sprite>();
		spriteUI_Num_[i]->Initialize(textureHandlesIntervalNum_[i]);
		spriteUI_Num_[i]->SetAnchorPoint({ 0.5f, 0.5f });
		spriteUI_Num_[i]->SetPosition({ 640.0f, 360.0f });
		spriteUI_Num_[i]->SetSize({ 0.0f, 0.0f });
		spriteUI_Num_[i]->SetColor({ 0.863f, 0.706f, 0.157f, 1.0f });
	}

	remainingSpawnNum_ = std::make_unique<RemainingSpawnNum>();
	remainingSpawnNum_->Initialize(kMaxSpawnNum);

	SetupCamera();

	//カメラの生成と初期化
	camera_ = std::make_unique<BaseCamera>();
	camera_->Initialize();
	camera_->SetRotate({ cameraRotate });
	camera_->SetTranslate(cameraTranslate);
	camera_->SetFarClip(200.0f);
	//パーティクルマネージャーにカメラをセット
	ParticleManager::GetInstance()->SetCamera(camera_.get());

	cameraControl_ = std::make_unique<CameraControl>();
	cameraControl_->Initialize(cameraRotate);

	// 当たり判定
	appColliderManager_ = AppColliderManager::GetInstance();
	appColliderManager_->Initialize();

	// プレイヤー
	playerManager_ = std::make_unique<PlayerManager>();
	// 出現位置設定
	SetupPlayerSpawnPositions();
	// 処理変数セット
	playerManager_->SetCharge(charge_.get());
	playerManager_->SetOnPlayerAdded([this]() {
		playerNum_++;
		});
	playerManager_->SetOnPlayerRemoved([this]() {
		playerNum_--;
		});
	playerManager_->Initialize();
	

	//エネミーマネージャーの生成と初期化
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(camera_.get(), &playerManager_->GetPlayers(), "enemy", "Fan","Freeze");

	SetupEnemyManager();
	
	//スカイドーム
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();
	// フィールド
	field_ = std::make_unique<Field>();
	field_->Initialize();

	SetupField();

	CreateObstacles();

	CreateBumpers();

	CreateIceFloors();

	// プレイヤースポーン位置
	playerSpawnManager_ = std::make_unique<PlayerSpawnManager>();
	playerSpawnManager_->SetCharge(charge_.get());
	playerSpawnManager_->SetPlayerSpawnPosition(stageSpawnPositions_);
	playerSpawnManager_->SetMaxSpawnNum(kMaxSpawnNum);
	playerSpawnManager_->Initialize();



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
	playerManager_->Finalize();

	playerSpawnManager_->Finalize();

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
	
	//チャージの更新
	charge_->Update();

	// 残り出現数の更新
	remainingSpawnNum_->Update(playerSpawnManager_->GetHowManyBoogie());

	// インターバルの数字の更新
	for (size_t i = 0; i < spriteUI_Num_.size(); ++i) {
		spriteUI_Num_[i]->Update();
	}

	StartInterVal();

	// カメラの更新
	UpdateCamera();

	// プレイヤーの更新
	playerManager_->Update();

	if (isGameStart_)
	{
		// プレイヤー攻撃チャージ
		charge_->playerTackleCharge(playerNum_ > 0, playerManager_->GetPlayers());
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
	playerSpawnManager_->Update();
	if (playerNum_ > 0 && isGameStart_)
	{
		playerSpawnManager_->PlayerSpawnRotation();
	}
	if (auto newPlayer = playerSpawnManager_->TryExportSpawnedPlayer()) 
	{
		playerManager_->AddPlayer(std::move(newPlayer));
		playerNum_++;
	}

	// 当たり判定
	appColliderManager_->CheckAllCollision();

	//ゲーム終了判定
	if ((playerNum_ <= 0 or enemyManager_->GetEnemyCount() == 0) and !isGameEnd_)
	{
		isGameEnd_ = true;

		cameraControl_->OnGameEnd(playerNum_, enemyManager_->GetEnemyCount());

	}
	// ゲームオーバーへ
	if (playerNum_ <= 0 and isGameEnd_ and cameraControl_->IsGameEnd())
	{
		sceneManager_->SetNextScene("GAMEOVER");
	}
	// クリア
	if (enemyManager_->GetEnemyCount() == 0 and isGameEnd_ and cameraControl_->IsGameEnd())
	{
		sceneManager_->SetNextScene("CLEAR");
	}

	//揺らす処理
	CheckShake();

	// ImGui
	ImGuiDraw();

	remainingSpawnNum_->DebugWithImGui();
}

void GamePlayScene::Draw()
{
	//3Dモデルの共通描画設定
	Object3dCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓モデル描画開始↓↓↓↓
	///------------------------------///

	// 通常プレイヤーの描画
	playerManager_->Draw(*camera_.get());

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
	playerSpawnManager_->Draw(*camera_.get());


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
	
	//チャージの描画
	charge_->Draw();

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
		remainingSpawnNum_->TextDraw();
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
	
	// プレイヤー関連
	playerManager_->ImGuiDraw();

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

	ImGui::Text("charge : % .0f", charge_->GetChargeValue());


	ImGui::End();

	// プレイヤー
	playerManager_->ImGuiDraw();	

	// フィールド
	field_->ImGuiDraw();

	// スポーン位置
	playerSpawnManager_->ImGuiDraw();

	//ポーズシステム
	pauseSystem_->DebugWithImGui();

#endif // _DEBUG

}

void GamePlayScene::CheckShake() {
	//全てのプレイヤーのシェイク判定を処理
	for (auto& player : playerManager_->GetPlayers()) {
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

void GamePlayScene::UpdateTransform()
{
	// プレイヤーの位置を更新
	playerManager_->UpdateTransform();
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
	if (!pauseSystem_->GetIsPause()) {
		if (gameStartDelayTimer_ <= 3.0f && gameStartDelayTimer_ > 2.0f) {
			spriteUI_Num_[2]->Draw(); // 3
		} else if (gameStartDelayTimer_ <= 2.0f && gameStartDelayTimer_ > 1.0f) {
			spriteUI_Num_[1]->Draw(); // 2
		} else if (gameStartDelayTimer_ <= 1.0f && gameStartDelayTimer_ > 0.0f) {
			spriteUI_Num_[0]->Draw(); // 1
		}
	}

	// 数字のサイズを更新
	for (auto& sprite : spriteUI_Num_) {
		sprite->SetSize(numSize_);
	}

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

void GamePlayScene::StartInterVal()
{
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
		playerManager_->IsMoveable(false);

		// エネミーの行動不能フラグセット
		enemyManager_->IsMoveable(false);
	}
	else
	{
		playerManager_->IsMoveable(true);

		enemyManager_->IsMoveable(true);
	}
}

void GamePlayScene::SetupCamera()
{
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

}

void GamePlayScene::SetupPlayerSpawnPositions()
{
	// スポーン位置
	switch (stageNum_)
	{
	case 1:
		stageSpawnPositions_.push_back({ 0.0f,1.0f,5.0f });
		stageSpawnPositions_.push_back({ 5.0f,1.0f,-5.0f });
		stageSpawnPositions_.push_back({ -5.0f,1.0f,-5.0f });
		break;

	case 2:
		stageSpawnPositions_.push_back({ 5.0f,1.0f,5.0f });
		stageSpawnPositions_.push_back({ -5.0f,1.0f,5.0f });
		stageSpawnPositions_.push_back({ 0.0f,1.0f,-5.0f });
		break;

	case 3:
		stageSpawnPositions_.push_back({ 0.0f,1.0f,0.0f });
		stageSpawnPositions_.push_back({ 2.0f,1.0f,2.0f });
		stageSpawnPositions_.push_back({ -2.0f,1.0f,-2.0f });
		break;

	case 4:
		stageSpawnPositions_.push_back({ 0.0f,1.0f,13.0f });
		stageSpawnPositions_.push_back({ 8.0f,1.0f,-10.0f });
		stageSpawnPositions_.push_back({ -10.0f,1.0f,-10.0f });
		break;

	case 5:
		stageSpawnPositions_.push_back({ 0.0f,1.0f,0.0f });
		stageSpawnPositions_.push_back({ 2.0f,1.0f,2.0f });
		stageSpawnPositions_.push_back({ -2.0f,1.0f,-2.0f });
		break;

	default:
		break;
	}

	playerManager_->SetSpawnPositions(stageSpawnPositions_);

}

void GamePlayScene::SetupEnemyManager()
{
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
}

void GamePlayScene::SetupField()
{
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
}

void GamePlayScene::CreateObstacles()
{
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

}

void GamePlayScene::CreateBumpers()
{
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
}

void GamePlayScene::CreateIceFloors()
{
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
}

void GamePlayScene::UpdateCamera()
{
	cameraControl_->Update(camera_.get(), playerManager_.get(), enemyManager_.get());
	// カメラの行列を更新
	camera_->UpdateMatrix();
}
