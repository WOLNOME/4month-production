#include "GamePlayScene2.h"

#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "LineDrawerCommon.h"
#include "SpriteCommon.h"
#include "SceneManager.h"
#include <numbers>

void GamePlayScene2::Initialize()
{
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

	// スプライト
	textureHandleUI_PLAY_ = TextureManager::GetInstance()->LoadTexture("UI_PLAY.png");
	spriteUI_PLAY_ = std::make_unique<Sprite>();
	spriteUI_PLAY_->Initialize(textureHandleUI_PLAY_);

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
	playerSpawnPositions_.push_back({ 5.0f,1.0f,5.0f });
	playerSpawnPositions_.push_back({ -5.0f,1.0f,5.0f });
	playerSpawnPositions_.push_back({ 0.0f,1.0f,-5.0f });

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
	enemyManager_->Initialize(camera_.get(), &players_, "enemy", "Fan", "Freeze");
	enemyManager_->SpawnTackleEnemy(4);
	enemyManager_->SpawnFanEnemy(3);

	//スカイドーム
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();
	// フィールド
	field_ = std::make_unique<Field>();
	field_->Initialize();
	field_->SetScale({ 20.0f,1.0f,20.0f });


	//障害物の生成
	std::vector<Vector3> obstaclePositions = {
		{ 0.0f, 1.0f, 7.0f },
		{ -15.0f, 1.0f, 0.0f },
		{ 10.0f, 1.0f, -15.0f }
	};
	std::vector<Vector3> obstacleScales = {
		{ 5.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 5.0f },
		{ 1.0f, 1.0f, 5.0f }
	};

	for (size_t i = 0; i < obstaclePositions.size(); ++i)
	{
		std::unique_ptr<Obstacle>& obstacle = obstacles_.emplace_back();
		obstacle = std::make_unique<Obstacle>();
		obstacle->Initialize();
		obstacle->SetPosition(obstaclePositions[i]);
		obstacle->SetScale(obstacleScales[i]);
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

void GamePlayScene2::Finalize()
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
}

void GamePlayScene2::Update()
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
	if (playerNum_ <= 0 or input_->TriggerKey(DIK_RETURN))
	{
		sceneManager_->SetNextScene("GAMEOVER");
	}
	// クリア
	if (input_->TriggerKey(DIK_TAB) or enemyManager_->GetEnemyCount() == 0)
	{
		sceneManager_->SetNextScene("CLEAR");
	}


	// ImGui
	ImGuiDraw();
}

void GamePlayScene2::Draw()
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

void GamePlayScene2::TextDraw()
{
	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///



	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///
}

void GamePlayScene2::ImGuiDraw()
{

#ifdef _DEBUG

	ImGui::Begin("scene");
	ImGui::Text("%s", "GAMEPLAY2");;

	ImGui::Text("%s", "ToClear : TAB");
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



#endif // _DEBUG
}

void GamePlayScene2::playerSpawnRotation()
{
	// プレイヤースポーン位置のローテーション
	rotationTimer_ -= 1.0f;
	if (rotationTimer_ <= 0.0f && howManyBoogie_ < 30)
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

void GamePlayScene2::playerTackleCharge()
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
