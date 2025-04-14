#include "GamePlayScene.h"

#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "LineDrawerCommon.h"
#include "SpriteCommon.h"
#include "SceneManager.h"
#include <numbers>

uint32_t GamePlayScene::stageNum_ = 1;

void GamePlayScene::Initialize()
{
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

	// スプライト
	textureHandleUI_PLAY_ = TextureManager::GetInstance()->LoadTexture("UI_PLAY.png");
	spriteUI_PLAY_ = std::make_unique<Sprite>();
	spriteUI_PLAY_->Initialize(textureHandleUI_PLAY_);

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
		rotation_ = 300.0f;
		break;

	case 2:
		playerSpawnPositions_.push_back({ 5.0f,1.0f,5.0f });
		playerSpawnPositions_.push_back({ -5.0f,1.0f,5.0f });
		playerSpawnPositions_.push_back({ 0.0f,1.0f,-5.0f });
		rotation_ = 120.0f;
		break;

	case 3:
		playerSpawnPositions_.push_back({ 0.0f,1.0f,0.0f });
		playerSpawnPositions_.push_back({ 2.0f,1.0f,2.0f });
		playerSpawnPositions_.push_back({ -2.0f,1.0f,-2.0f });
		rotation_ = 120.0f;
		break;

	case 4:
		playerSpawnPositions_.push_back({ 0.0f,1.0f,13.0f });
		playerSpawnPositions_.push_back({ 8.0f,1.0f,-10.0f });
		playerSpawnPositions_.push_back({ -10.0f,1.0f,-10.0f });
		rotation_ = 120.0f;
		break;

	case 5:
		playerSpawnPositions_.push_back({ 0.0f,1.0f,0.0f });
		playerSpawnPositions_.push_back({ 2.0f,1.0f,2.0f });
		playerSpawnPositions_.push_back({ -2.0f,1.0f,-2.0f });
		rotation_ = 120.0f;
		break;
	
	default:
		break;
	}
	

	// プレイヤー
	for (uint32_t i = 0; i < 1; ++i)
	{
		auto player = std::make_unique<Player>();

		player->SetPlayerPos(playerSpawnPositions_[0]);
		player->Initialize();

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
}

void GamePlayScene::Finalize()
{
	for (auto& player : players_)
	{
		player->Finalize();
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
}

void GamePlayScene::Update()
{
	// スプライト
	spriteUI_PLAY_->Update();

	// カメラの更新
	camera_->UpdateMatrix();
	camera_->SetRotate({ cameraRotate });
	camera_->SetTranslate(cameraTranslate);

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
	// プレイヤー
	for (auto& player : players_)
	{
		player->Update();
	}
	// プレイヤー攻撃チャージ
	playerTackleCharge();


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
	if (playerNum_ > 0)
	{
		playerSpawnRotation();
	}

	// 当たり判定
	appCollisionManager_->CheckAllCollision();

	//タイトルシーンに戻る
	if (input_->TriggerKey(DIK_ESCAPE)) {
		sceneManager_->SetNextScene("TITLE");
	}

	// ゲームオーバーへ
	if (playerNum_ <= 0  or input_->TriggerKey(DIK_RETURN))
	{
		sceneManager_->SetNextScene("GAMEOVER");
	}
	// クリア
	if (input_ ->TriggerKey(DIK_TAB) or enemyManager_->GetEnemyCount() == 0)
	{
		sceneManager_->SetNextScene("CLEAR");
	}

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

	///------------------------------///
	///↑↑↑↑スプライト描画終了↑↑↑↑
	///------------------------------///
}

void GamePlayScene::TextDraw() {
	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///



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

#endif // _DEBUG

}

void GamePlayScene::playerSpawnRotation()
{
	// プレイヤースポーン位置のローテーション
	rotationTimer_ -= 1.0f;
	if (rotationTimer_ <= 0.0f && howManyBoogie_ < 15)
	{
		rotationTimer_ = rotation_;

		// プレイヤーを追加
		auto player = std::make_unique<Player>();
		howManyBoogie_++;

		player->SetPlayerPos(playerSpawnPositions_[playerSpawnIndex_]);
		player->Initialize();

		players_.push_back(std::move(player));

		playerNum_++;

		// 位置ローテを0に戻す
		playerSpawnIndex_++;
		if (playerSpawnIndex_ > playerSpawnNum_ - 1)
		{
			playerSpawnIndex_ = 0;
		}
	}
}

void GamePlayScene::playerTackleCharge()
{
	// プレイヤーが1体以上いるとき
	if (playerNum_ > 0)
	{
		// プレイヤーの攻撃フラグが立っているとき
		if (!players_[0]->IsChargeMax())
		{
			charge_ += 1.0f;
		}

		// チャージが最大値に達したら
		if (charge_ >= chargeMax_)
		{
			for (auto& player : players_)
			{
				// 攻撃できるようにする
				player->SetIsChargeMax(true);
			}

			charge_ = 0.0f;
		}

	}
}
