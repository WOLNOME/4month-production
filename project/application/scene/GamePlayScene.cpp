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

	ChangeState(std::make_unique<StartState>());

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
	textureHandleIntervalNum_.resize(3);
	textureHandleIntervalNum_[0] = TextureManager::GetInstance()->LoadTexture("count1.png");
	textureHandleIntervalNum_[1] = TextureManager::GetInstance()->LoadTexture("count2.png");
	textureHandleIntervalNum_[2] = TextureManager::GetInstance()->LoadTexture("count3.png");

	interval_ = std::make_unique<Interval>();
	interval_->Initialize(textureHandleIntervalNum_);

	remainingSpawnNum_ = std::make_unique<RemainingSpawnNum>();
	remainingSpawnNum_->Initialize(kMaxSpawnNum);

	SetupCamera();

	//カメラの生成と初期化
	camera_ = std::make_unique<GameCamera>();
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
	// プレイヤースポーン位置
	playerSpawnManager_ = std::make_unique<PlayerSpawnManager>();
	// 出現位置設定
	SetupPlayerSpawnPositions();
	// 処理変数セット
	// プレイヤーマネージャー
	playerManager_->SetCharge(charge_.get());
	playerManager_->SetOnPlayerAdded([this]() {
		playerNum_++;
		});
	playerManager_->SetOnPlayerRemoved([this]() {
		playerNum_--;
		});
	playerManager_->Initialize();
	// プレイヤースポーンマネージャー
	playerSpawnManager_->SetCharge(charge_.get());
	playerSpawnManager_->SetMaxSpawnNum(kMaxSpawnNum);
	playerSpawnManager_->Initialize();


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



	//bgm
	bgm_ = std::make_unique<Audio>();
	bgm_->Initialize("game/bgm.wav");
	bgm_->Play(true, 0.15f);

	//ポーズシステム
	pauseSystem_ = std::make_unique<PauseSystem>();
	pauseSystem_->Initialize();

	//エネミーにゲームカメラを渡す
	enemyManager_->SetGameCamera(camera_.get());
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

	interval_->SpriteUpdate();

	// カメラの更新
	UpdateCamera();

	// プレイヤーの更新
	playerManager_->Update();

	// プレイヤースポーンマネージャーの更新
	playerSpawnManager_->Update();

	//状況に応じた更新
	if (currentState_)
	{
		currentState_->Update(this);
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

	if (!pauseSystem_->GetIsPause()) {
		interval_->Draw();
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

void GamePlayScene::ChangeState(std::unique_ptr<GamePlayState> newState)
{
	currentState_ = std::move(newState);
}
	

void GamePlayScene::UpdateIntervalNum()
{
	interval_->UpdateIntervalNum();
}

void GamePlayScene::StartInterVal()
{
	// ゲーム開始のインターバル
	
	// 初期モデルだけ 1 回 Update しておく
	if (!interval_->GetHasPreUpdated()) {
		// ここで全プレイヤー・敵・エフェクトの Update を呼ぶ
		UpdateTransform();

		interval_->SetHasPreUpdated(true);
	}

	// プレイヤーの行動不能フラグセット
	playerManager_->IsMoveable(false);

	// エネミーの行動不能フラグセット
	enemyManager_->IsMoveable(false);
	

}

void GamePlayScene::ObjectsUpdate()
{
	// プレイヤーの更新
	playerManager_->Update();

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

	// 当たり判定
	appColliderManager_->CheckAllCollision();
}

void GamePlayScene::TackleCharge()
{
	// プレイヤー攻撃チャージ
	charge_->playerTackleCharge(playerNum_ > 0, playerManager_->GetPlayers());
}

void GamePlayScene::playerSpawnRotation()
{
	// プレイヤーのスポーン位置での回転処理
	playerSpawnManager_->PlayerSpawnRotation();
	// スポーンしたプレイヤーを取得
	if (auto newPlayer = playerSpawnManager_->TryExportSpawnedPlayer())
	{
		playerManager_->AddPlayer(std::move(newPlayer));
		playerNum_++;
	}
}

bool GamePlayScene::IsStartConditionMet()
{
	return interval_->IsEndInterval();
}

bool GamePlayScene::IsGameEnd()
{
	return playerNum_ <= 0 or enemyManager_->GetEnemyCount() == 0;
}

bool GamePlayScene::IsGameClear()
{
	return enemyManager_->GetEnemyCount() == 0 and cameraControl_->IsGameEnd();
}

void GamePlayScene::GameClearProcess()
{
	sceneManager_->SetNextScene("CLEAR");
}

bool GamePlayScene::IsGameOver()
{
	return playerNum_ <= 0 and cameraControl_->IsGameEnd();
}

void GamePlayScene::GameEndProcess()
{
	cameraControl_->OnGameEnd(playerNum_, enemyManager_->GetEnemyCount());
}

void GamePlayScene::GameOverProcess()
{
	sceneManager_->SetNextScene("GAMEOVER");
}

void GamePlayScene::ObjectsMoveable()
{
	playerManager_->IsMoveable(true);

	enemyManager_->IsMoveable(true);
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
			playerSpawnManager_->SetPlayerSpawnPosition(LoadVector3(lineStream));
			//playerManager_->SetSpawnPositions(LoadVector3(lineStream));
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

void GamePlayScene::UpdateCamera()
{
	cameraControl_->Update(camera_.get(), playerManager_.get(), enemyManager_.get());
	// カメラの行列を更新
	camera_->UpdateMatrix();
}
