#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Audio.h"
#include "Vector2.h"
#include "Input.h"
#include "LineDrawer.h"
#include "DevelopCamera.h"

#include <cstdint>
#include <memory>

// アプリケーションインクルード
#include "../../application/objects/Player/PlayerManager.h"
#include "../../application/objects/Player/SpawnPos/PlayerSpawnManager.h"
#include "../../application/objects/Skydome/Skydome.h"
#include "../../application/objects/Field/Field.h"
#include "../appCollider/AppColliderManager.h"
#include "../../application/enemy/EnemyManager.h"
#include "../../application/objects/Gimmick/Obstacle.h"
#include "../../application/objects/Gimmick/Bumper.h"
#include "../../application/objects/Gimmick/IceFloor.h"
#include "../../application/PauseSystem.h"
#include "../../application/UI/Charge.h"
#include "../../application/UI/RemainingSpawnNum.h"
#include "../../application/CameraControl/CameraControl.h"
#include "GamePlayState.h"
#include "../../application/UI/Interval.h"

class GamePlayScene : public BaseScene
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() override;
	/// <summary>
	/// 終了時
	/// </summary>
	void Finalize() override;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() override;
	/// <summary>
	/// テキスト描画
	/// </summary>
	virtual void TextDraw() override;

	//ステージ番号
	static uint32_t stageNum_;

	void ChangeState(std::unique_ptr<GamePlayState> newState);

	// ステージ始まるインターバル数字
	virtual void UpdateIntervalNum();

	virtual void StartInterVal();

	void ObjectsUpdate();

	void TackleCharge();

	// プレイヤースポーン処理
	void playerSpawnRotation();

	bool IsStartConditionMet();

	//ゲーム終了判定
	bool IsGameEnd();

	//ゲーム終了時の処理
	void GameEndProcess();

	//ゲームクリア判定
	bool IsGameClear();

	//ゲームクリア時の処理
	void GameClearProcess();

	//ゲームオーバー判定
	bool IsGameOver();

	//ゲームオーバー時の処理
	void GameOverProcess();

	void ObjectsMoveable();

private: //メンバ関数

	// ImGui
	void ImGuiDraw();

	// カメラの更新
	void UpdateCamera();

	//シェイクのチェック
	void CheckShake();

	// モデルの更新(インターバル中に位置だけでもおいておきたい)
	void UpdateTransform();

	// カメラのセットアップ
	void SetupCamera();

	//プレイヤーのスポーン位置のセットアップ
	void SetupPlayerSpawnPositions();

	//エネミーマネージャーのセットアップ
	void SetupEnemyManager();

	//フィールドのセットアップ
	void SetupField();

	//障害物の生成
	void CreateObstacles();

	//跳ね返る障害物の生成
	virtual void CreateBumpers();

	//氷の床の生成
	virtual void CreateIceFloors();

	virtual void SetupCsvFilePath();

	// CSVファイルの読み込み。　filenameに拡張子はつけないこと
	std::stringstream LoadCsvFile(std::string fileName);

	Vector3 LoadVector3(std::istringstream& lineStream);

	int LoadInt(std::istringstream& lineStream);

	float LoadFloat(std::istringstream& lineStream);

	bool LoadBool(std::istringstream& lineStream);

	//プレイヤーの追加
	void AddPlayer(bool preSpawn);

protected://メンバ変数

	std::unique_ptr<GamePlayState> currentState_;

	Input* input_ = nullptr;
	

	// プライト
	// プレイ画面UI
	uint32_t textureHandleUI_PLAY_ = 0u;
	std::unique_ptr<Sprite> spriteUI_PLAY_ = nullptr;
	
	std::unique_ptr<Charge> charge_ = nullptr;

	// インターバルの数字
	std::vector<uint32_t> textureHandleIntervalNum_{};
	
	//インターバル
	std::unique_ptr<Interval> interval_ = nullptr;

	std::unique_ptr<RemainingSpawnNum> remainingSpawnNum_;

	std::unique_ptr<BaseCamera> camera_ = nullptr;

	Vector3 cameraTranslate = { 0.0f,70.0f,-50.0f };
	Vector3 cameraRotate = { 0.95f,0.0f,0.0f };

	// 当たり判定
	AppColliderManager* appColliderManager_ = nullptr;

	// プレイヤー
	std::unique_ptr<PlayerManager> playerManager_ = nullptr;

	// プレイヤースポーンマネージャー
	std::unique_ptr<PlayerSpawnManager> playerSpawnManager_ = nullptr;

	//エネミーマネージャー
	std::unique_ptr<EnemyManager> enemyManager_;

	//スカイドーム
	std::unique_ptr<Skydome> skydome_ = nullptr;
	// フィールド
	std::unique_ptr<Field> field_ = nullptr;

	//障害物
	std::vector<std::unique_ptr<Obstacle>> obstacles_;

	//跳ね返る障害物
	std::vector<std::unique_ptr<Bumper>> bumpers_;

	//氷の床
	std::vector<std::unique_ptr<IceFloor>> icefloors_;

	//bgm
	std::unique_ptr<Audio> bgm_ = nullptr;

	//ポーズシステム
	std::unique_ptr<PauseSystem> pauseSystem_ = nullptr;

	// プレイヤー出現ポイント
	std::vector<Vector3> stageSpawnPositions_;
	// 出現数上限
	const uint32_t kMaxSpawnNum = 15;

	// フィールド上にいるプレイヤーの数
	uint32_t playerNum_ = 0;

	std::unique_ptr<CameraControl> cameraControl_ = nullptr;

	std::string csvFilePath_;

};