#include "GamePlayScene4.h"

#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "LineDrawerCommon.h"
#include "SpriteCommon.h"
#include "SceneManager.h"
#include <numbers>
#include "application/MathUtils.h"

void GamePlayScene4::Initialize()
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
	playerSpawnPositions_.push_back({ 0.0f,1.0f,13.0f });
	playerSpawnPositions_.push_back({ 8.0f,1.0f,-10.0f });
	playerSpawnPositions_.push_back({ -10.0f,1.0f,-10.0f });

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
	enemyManager_->SpawnTackleEnemy(7);
	enemyManager_->SpawnFanEnemy(5);

	//スカイドーム
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	// フィールド
	field_ = std::make_unique<Field>();
	field_->Initialize();
	field_->SetScale({ 30.0f, 1.0f, 30.0f });
	//フィールドの大きさに合わせて敵のスポーン範囲を設定
	enemyManager_->SetSpawnPosition({ -30.0f,1.0f,-30.0f }, { 30.0f,1.0f,30.0f });

	//跳ね返る障害物の生成
	std::vector<Vector3> bumperPositions = {
		{ 0.0f, 1.0f, 12.0f },
		{ -20.0f, 1.0f, 0.0f },
		{ 17.0f, 1.0f, -22.0f }
	};
	std::vector<Vector3> bumperScales = {
		{ 3.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 3.0f },
		{ 1.0f, 1.0f, 3.0f }
	};

	for (size_t i = 0; i < bumperPositions.size(); ++i)
	{
		std::unique_ptr<Bumper>& bumper = bumpers_.emplace_back();
		bumper = std::make_unique<Bumper>();
		bumper->Initialize();
		bumper->SetPosition(bumperPositions[i]);
		bumper->SetScale(bumperScales[i]);
	}

	// プレイヤースポーン位置モデル
	for (uint32_t i = 0; i < playerSpawnNum_; ++i)
	{
		auto playerSpawn = std::make_unique<SpawnPos>();
		playerSpawn->SetPosition(playerSpawnPositions_[i]);
		playerSpawn->Initialize();

		playerSpawn_.push_back(std::move(playerSpawn));
	}

	//ポーズシステム
	pauseSystem_ = std::make_unique<PauseSystem>();
	pauseSystem_->Initialize();
}

void GamePlayScene4::Finalize()
{
	for (auto& player : players_)
	{
		player->Finalize();
	}

	enemyManager_->Finalize();

	field_->Finalize();

	for (std::unique_ptr<Bumper>& bumper : bumpers_)
	{
		bumper->Finalize();
	}

}

void GamePlayScene4::Update()
{
	//ポーズシステムの更新
	pauseSystem_->Update();
	if (pauseSystem_->GetIsPause()) {
		return;
	}

	// スプライト
	spriteUI_PLAY_->Update();

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
	// プレイヤー
	for (auto& player : players_)
	{
		player->Update();
	}


	//エネミーマネージャーの更新
	enemyManager_->Update();

	//スカイドーム
	skydome_->Update();
	// フィールド
	field_->Update();

	//跳ね返る障害物
	for (std::unique_ptr<Bumper>& bumper : bumpers_)
	{
		bumper->Update();
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
}

void GamePlayScene4::Draw()
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

	//跳ね返る障害物
	for (std::unique_ptr<Bumper>& bumper : bumpers_)
	{
		bumper->Draw(*camera_.get());
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
	//ポーズシステムの描画
	pauseSystem_->DrawSprite();

	///------------------------------///
	///↑↑↑↑スプライト描画終了↑↑↑↑
	///------------------------------///
}

void GamePlayScene4::TextDraw()
{
	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///

	// ポーズシステムのテキスト描画
	pauseSystem_->TextDraw();

	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///
}

void GamePlayScene4::ImGuiDraw()
{
#ifdef _DEBUG

	ImGui::Begin("scene");
	ImGui::Text("%s", "GAMEPLAY4");;

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

	ImGui::Text("howManyBoogie : %d", howManyBoogie_);

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

void GamePlayScene4::playerSpawnRotation()
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

void GamePlayScene4::UpdateCamera()
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

void GamePlayScene4::UpdateZoomIn()
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
	cameraEndPosition.y = 3.0f;
	cameraEndPosition.z -= 10.0f;
	// カメラの補間
	Vector3 newPosition = cameraStartPosition_ * (1.0f - easeT) + cameraEndPosition * easeT;
	Vector3 newRotation = cameraRotate * (1.0f - easeT) + CalculateLookAtRotation(newPosition, cameraEndPosition_) * easeT; // カメラの向きを補間

	// 敵の向きを補間
	if (!nearestEnemyType_.empty() and enemyManager_->GetEnemyCount() != 0)
	{
		Vector3 rotation = { 0.0f, 0.0f, 0.0f };
		//敵の現在の向き
		if (nearestEnemyType_ == "TackleEnemy")
		{
			rotation = enemyManager_->GetTackleEnemy(nearestEnemyNum_)->GetRotation();
		}
		else if (nearestEnemyType_ == "FanEnemy")
		{
			rotation = enemyManager_->GetFanEnemy(nearestEnemyNum_)->GetRotation();
		}
		else if (nearestEnemyType_ == "FreezeEnemy")
		{
			rotation = enemyManager_->GetFreezeEnemy(nearestEnemyNum_)->GetRotation();
		}
		Vector3 newEnemyRotate = rotation * (1.0f - easeT) + CalculateLookAtRotation(nearestEnemyPos_, cameraEndPosition_) * easeT;
		enemyManager_->GetTackleEnemy(nearestEnemyNum_)->SetRotation(newEnemyRotate);
	}

	// プレイヤーの向きを補間
	if (nearestPlayerNum_ != (std::numeric_limits<uint32_t>::max)() and !players_.empty())
	{
		Vector3 rotation = players_[nearestPlayerNum_]->GetRotation();
		Vector3 newPlayerRotate = rotation * (1.0f - easeT) + CalculateLookAtRotation(cameraEndPosition, nearestPlayerPos_) * easeT;
		players_[nearestPlayerNum_]->SetRotation(newPlayerRotate);
	}

	// カメラに新しい位置と回転を設定
	camera_->SetTranslate(newPosition);
	camera_->SetRotate(newRotation);

	//補間が終わったら
	if (t >= 1.0f)
	{
		isZoomIn_ = false;
		cameraEaseTime_ = 0.0f;
		waitTime_ = waitTimeDuration_;
	}
}

void GamePlayScene4::UpdateZoomOut()
{
	// 時間を経過させる
	cameraEaseTime_ += 1.0f / 60.0f;
	float t = cameraEaseTime_ / cameraEaseDuration_;
	// 補完が1.0fを超えないようにする
	if (t > 1.0f) t = 1.0f;

	// 補完のイージング
	float easeT = EaseInOutQuad(t);
	Vector3 cameraEndPosition = cameraEndPosition_;
	cameraEndPosition.y = 3.0f;
	cameraEndPosition.z -= 10.0;
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

void GamePlayScene4::CalculateNearestPosition()
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

void GamePlayScene4::CheckShake() {
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