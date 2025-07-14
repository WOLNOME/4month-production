#include "GamePlayScene.h"

#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "LineDrawerCommon.h"
#include "SpriteCommon.h"
#include "SceneManager.h"
#include <numbers>
#include "application/MathUtils.h"

#include <fstream>

#include "../../application/UI/RemainingSpawnNum.h"


uint32_t GamePlayScene::stageNum_ = 1;

void GamePlayScene::Initialize()
{
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

	SetupCsvFilePath();

	// スプライト
	// プレイ画面UI
	textureHandleUI_PLAY_ = TextureManager::GetInstance()->LoadTexture("UI_PLAY.png");
	spriteUI_PLAY_ = std::make_unique<Sprite>();
	spriteUI_PLAY_->Initialize(textureHandleUI_PLAY_);

	charge_ = std::make_unique<Charge>();
	charge_->Initialize(TextureManager::GetInstance()->LoadTexture("spawn.png"));

	// インターバルの数字
	textureHandleIntervalNum1_ = TextureManager::GetInstance()->LoadTexture("count1.png");
	textureHandleIntervalNum2_ = TextureManager::GetInstance()->LoadTexture("count2.png");
	textureHandleIntervalNum3_ = TextureManager::GetInstance()->LoadTexture("count3.png");
	
	spriteUI_Num1_ = std::make_unique<Sprite>();
	spriteUI_Num1_->Initialize(textureHandleIntervalNum1_);
	spriteUI_Num1_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteUI_Num1_->SetPosition({ 640.0f, 360.0f });
	spriteUI_Num1_->SetSize({ 0.0f, 0.0f });
	spriteUI_Num1_-> SetColor({ 0.863f, 0.706f, 0.157f, 1.0f });

	spriteUI_Num2_ = std::make_unique<Sprite>();
	spriteUI_Num2_->Initialize(textureHandleIntervalNum2_);
	spriteUI_Num2_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteUI_Num2_->SetPosition({ 640.0f, 360.0f });
	spriteUI_Num2_->SetSize({ 0.0f, 0.0f });
	spriteUI_Num2_->SetColor({ 0.863f, 0.706f, 0.157f, 1.0f });
	
	spriteUI_Num3_ = std::make_unique<Sprite>();
	spriteUI_Num3_->Initialize(textureHandleIntervalNum3_);
	spriteUI_Num3_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteUI_Num3_->SetPosition({ 640.0f, 360.0f });
	spriteUI_Num3_->SetSize({ 0.0f, 0.0f });
	spriteUI_Num3_->SetColor({ 0.863f, 0.706f, 0.157f, 1.0f });

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
	appCollisionManager_ = AppCollisionManager::GetInstance();
	appCollisionManager_->Initialize();

	SetupPlayerSpawnPositions();

	// プレイヤー
	for (uint32_t i = 0; i < 1; ++i)
	{
		AddPlayer(false);
	}

	//エネミーマネージャーの生成と初期化
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(camera_.get(), &players_, "enemy", "Fan","Freeze");

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
	
	//チャージの更新
	charge_->Update();

	// 残り出現数の更新
	remainingSpawnNum_->Update(howManyBoogie_);

	spriteUI_Num1_->Update();
	spriteUI_Num2_->Update();
	spriteUI_Num3_->Update();

	StartInterVal();

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
		charge_->playerTackleCharge(playerNum_ > 0, players_);
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

	ImGui::SliderFloat3("playerSpawnPos", &playerSpawnPositions_[0].x, -10.0f, 10.0f);
	// プレイヤーを追加するボタン
	if (ImGui::Button("Add Player"))
	{
		AddPlayer(false);
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

	ImGui::Text("charge : % .0f", charge_->GetChargeValue());

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
		AddPlayer(true);

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
	if (!pauseSystem_->GetIsPause()) {
		if (gameStartDelayTimer_ <= 3.0f && gameStartDelayTimer_ > 2.0f) {
			spriteUI_Num3_->Draw();
		}
		else if (gameStartDelayTimer_ <= 2.0f && gameStartDelayTimer_ > 1.0f) {
			spriteUI_Num2_->Draw();
		}
		else if (gameStartDelayTimer_ <= 1.0f && gameStartDelayTimer_ > 0.0f) {
			spriteUI_Num1_->Draw();
		}
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
}

void GamePlayScene::SetupCamera()
{

	std::stringstream cameraDatas = LoadCsvFile("Camera");

	std::string line;

	// カメラの設定を読み込む
	while (std::getline(cameraDatas, line))
	{ 
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream lineStream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(lineStream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}

		//translate
		if (word.find("translate") == 0)
		{
			cameraTranslate = LoadVector3(lineStream);
		}
	}

}

void GamePlayScene::SetupPlayerSpawnPositions()
{
	std::stringstream cameraDatas = LoadCsvFile("PlayerSpawn");

	std::string line;

	// プレイヤーのスポーン位置を読み込む
	while (std::getline(cameraDatas, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream lineStream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(lineStream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}

		//translate
		if (word.find("translate") == 0)
		{
			playerSpawnPositions_.push_back(LoadVector3(lineStream));
		}
	}
}

void GamePlayScene::SetupEnemyManager()
{
	std::stringstream cameraDatas = LoadCsvFile("Enemy");

	std::string line;

	// エネミーのスポーン設定を読み込む
	while (std::getline(cameraDatas, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream lineStream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(lineStream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}

		//TackleEnemyのスポーン設定
		if (word.find("Tackle") == 0)
		{
			enemyManager_->SpawnTackleEnemy(LoadInt(lineStream));
		}
		//FanEnemyのスポーン設定
		else if (word.find("Fan") == 0)
		{
			enemyManager_->SpawnFanEnemy(LoadInt(lineStream));
		}
		//FreezeEnemyのスポーン設定
		else if (word.find("Freeze") == 0)
		{
			enemyManager_->SpawnFreezeEnemy(LoadInt(lineStream));
		}
	}

}

void GamePlayScene::SetupField()
{
	std::stringstream cameraDatas = LoadCsvFile("Field");

	std::string line;

	// エネミーのスポーン設定を読み込む
	while (std::getline(cameraDatas, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream lineStream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(lineStream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}

		//Fieldのスケール設定
		if (word.find("Scale") == 0)
		{
			Vector3 scale = LoadVector3(lineStream);
			field_->SetScale(scale);
			Vector3 spawnMin = { -scale.x, scale.y, -scale.z };
			enemyManager_->SetSpawnPosition(spawnMin, scale);

		}
		
	}
	
}

void GamePlayScene::CreateObstacles()
{
	//障害物の生成
	std::vector<Vector3> obstaclePositions;
	std::vector<Vector3> obstacleScales;

	std::stringstream cameraDatas = LoadCsvFile("Obstacle");

	std::string line;

	// エネミーのスポーン設定を読み込む
	while (std::getline(cameraDatas, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream lineStream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(lineStream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}

		//obstacleのposition設定
		if (word.find("translate") == 0)
		{
			obstaclePositions.push_back(LoadVector3(lineStream));
		}
		//obstacleのscale設定
		else if (word.find("scale") == 0)
		{
			obstacleScales.push_back(LoadVector3(lineStream));
		}

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
	bool isMove = false;

	std::stringstream cameraDatas = LoadCsvFile("Bumper");

	std::string line;

	// エネミーのスポーン設定を読み込む
	while (std::getline(cameraDatas, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream lineStream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(lineStream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}

		//bumperが動くかどうかの設定
		if (word.find("isMove") == 0)
		{
			isMove = LoadBool(lineStream);
		}
		//bumperのposition設定
		else if (word.find("translate") == 0)
		{
			bumperPositions.push_back(LoadVector3(lineStream));
		}
		//bumperのscale設定
		else if (word.find("scale") == 0)
		{
			bumperScales.push_back(LoadVector3(lineStream));
		}
		//bumperのmoveDirection設定
		else if (word.find("moveDirection") == 0)
		{
			bumperDirections.push_back(LoadVector3(lineStream));
		}
		//bumperのmoveSpeed設定
		else if (word.find("moveSpeed") == 0)
		{
			bumperSpeeds.push_back(LoadFloat(lineStream));
		}
		//bumperのmoveRange設定
		else if (word.find("moveRange") == 0)
		{
			bumperRanges.push_back(LoadFloat(lineStream));
		}

	}

	// 動くかどうかのフラグに応じて、Bumperを生成
	if (isMove)
	{
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
	}
	// 動かない場合は、通常のBumperを生成
	else
	{
		for (size_t i = 0; i < bumperPositions.size(); ++i)
		{
			std::unique_ptr<Bumper>& bumper = bumpers_.emplace_back();
			bumper = std::make_unique<Bumper>();
			bumper->Initialize();
			bumper->SetPosition(bumperPositions[i]);
			bumper->SetScale(bumperScales[i]);
		}
	}

}

void GamePlayScene::CreateIceFloors()
{
	//氷の床の生成
	std::vector<Vector3> iceFloorPositions;
	std::vector<Vector3> iceFloorScales;
	std::vector<Vector3> iceFloorRotations;

	std::stringstream cameraDatas = LoadCsvFile("IceFloor");

	std::string line;

	// エネミーのスポーン設定を読み込む
	while (std::getline(cameraDatas, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream lineStream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(lineStream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}

		//icefloorのposition設定
		if (word.find("translate") == 0)
		{
			iceFloorPositions.push_back(LoadVector3(lineStream));
		}
		//icefloorのscale設定
		else if (word.find("scale") == 0)
		{
			iceFloorScales.push_back(LoadVector3(lineStream));
		}
		//icefloorのrotation設定
		else if (word.find("rotate") == 0)
		{
			iceFloorRotations.push_back(LoadVector3(lineStream));
		}
		

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

void GamePlayScene::SetupCsvFilePath()
{
	csvFilePath_ = "./Resources/stageData/stage" + std::to_string(stageNum_) + "/";
}

std::stringstream GamePlayScene::LoadCsvFile(std::string fileName)
{
	// CSVファイルのパスを設定
	std::string filePath = csvFilePath_ + fileName + ".csv";

	// ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	std::stringstream csvStream;
	csvStream << file.rdbuf(); // ファイルの内容をストリームに読み込む

	//ファイルを閉じる
	file.close();

	return csvStream;
}

Vector3 GamePlayScene::LoadVector3(std::istringstream& lineStream)
{
	std::string word;

	getline(lineStream, word, ',');
	float x = std::stof(word);

	getline(lineStream, word, ',');
	float y = std::stof(word);

	getline(lineStream, word, ',');
	float z = std::stof(word);

	return Vector3(x, y, z);
}

int GamePlayScene::LoadInt(std::istringstream& lineStream)
{
	std::string word;

	getline(lineStream, word, ',');
	return std::stoi(word);

}

float GamePlayScene::LoadFloat(std::istringstream& lineStream)
{
	std::string word;

	getline(lineStream, word, ',');
	return std::stof(word);
}

bool GamePlayScene::LoadBool(std::istringstream& lineStream)
{
	std::string word;

	getline(lineStream, word, ',');
	return (word == "1" || word == "true" || word == "TRUE");

}

void GamePlayScene::AddPlayer(bool preSpawn)
{
	auto player = std::make_unique<Player>();

	if (!preSpawn) 
	{
		player->SetPlayerPos(playerSpawnPositions_[0]);
		// プレイヤーがスポーンする場合は通常のスケール
		player->SetScale({ 1.0f, 1.0f, 1.0f });
	} 
	else 
	{
		player->SetPosition(playerSpawnPositions_[playerSpawnIndex_]);
		// スポーン準備中のプレイヤーは小さいスケール
		player->SetScale({ 0.1f, 0.1f, 0.1f });
	}
	player->Initialize();
	player->SetIsChargeMax(charge_->IsChargeMaxPtr());

	if (!preSpawn) 
	{
		players_.push_back(std::move(player));
		playerNum_++;

		return;
	}
	else 
	{
		player->IsMoveable(false);

		preSpawnedPlayer_ = std::move(player);

		preSpawnedPlayer_->Update();

		return;
	}
	
}

void GamePlayScene::UpdateCamera()
{
	cameraControl_->Update(camera_.get(), players_, enemyManager_.get());
	// カメラの行列を更新
	camera_->UpdateMatrix();
}
