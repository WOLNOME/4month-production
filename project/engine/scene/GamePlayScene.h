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
#include "../../application/objects/Player/Player.h"
#include "../../application/objects/Player/SpawnPos/SpawnPos.h"
#include "../../application/objects/Skydome/Skydome.h"
#include "../../application/objects/Field/Field.h"
#include "../../application/appCollider/AppCollisionManager.h"
#include "../../application/enemy/EnemyManager.h"
#include "../../application/objects/Gimmick/Obstacle.h"
#include "../../application/objects/Gimmick/Bumper.h"
#include "../../application/objects/Gimmick/IceFloor.h"
#include "../../application/PauseSystem.h"

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

	// ImGui
	void ImGuiDraw();

	// プレイヤースポーン処理
	void playerSpawnRotation();

	// カメラの更新
	void UpdateCamera();
	// カメラのズームイン
	void UpdateZoomIn();
	// カメラのズームアウト
	void UpdateZoomOut();
	// 最も近い敵やプレイヤーの位置を計算
	void CalculateNearestPosition();

	// Player攻撃チャージ
	void playerTackleCharge();

	//ステージ番号
	static uint32_t stageNum_;

	//シェイクのチェック
	void CheckShake();

	// 残り出現数UI
	void remainingSpawnNum();

	// モデルの更新(インターバル中に位置だけでもおいておきたい)
	void UpdateTransform();

	// ステージ始まるインターバル数字
	void UpdateIntervalNum();

protected: //メンバ関数

	// カメラのセットアップ
	virtual void SetupCamera();

	//プレイヤーのスポーン位置のセットアップ
	virtual void SetupPlayerSpawnPositions();

	//エネミーマネージャーのセットアップ
	virtual void SetupEnemyManager();

	//フィールドのセットアップ
	virtual void SetupField();

	//障害物の生成
	virtual void CreateObstacles();

	//跳ね返る障害物の生成
	virtual void CreateBumpers();

	//氷の床の生成
	virtual void CreateIceFloors();

protected://メンバ変数

	Input* input_ = nullptr;
	

	// プライト
	// プレイ画面UI
	uint32_t textureHandleUI_PLAY_ = 0u;
	std::unique_ptr<Sprite> spriteUI_PLAY_ = nullptr;
	
	// チャージUI
	uint32_t textureHandleUI_Charge_ = 0u;
	std::unique_ptr<Sprite> spriteUI_Charge_ = nullptr;
	std::unique_ptr<Sprite> spriteUI_ChargeGage_ = nullptr;
	
	// インターバルの数字
	uint32_t textureHandleIntervalNum1_ = 0u;
	uint32_t textureHandleIntervalNum2_ = 0u;
	uint32_t textureHandleIntervalNum3_ = 0u;
	std::unique_ptr<Sprite> spriteUI_Num1_ = nullptr;
	std::unique_ptr<Sprite> spriteUI_Num2_ = nullptr;
	std::unique_ptr<Sprite> spriteUI_Num3_ = nullptr;

	
	// 残り出現数
	uint32_t remainingBoogie_ = 0;
	// 残りの出現数テキスト
	std::unique_ptr<TextWrite> remainingSpawnNumText_ = nullptr;
	std::unique_ptr<TextWrite> numText_ = nullptr;
	std::unique_ptr<TextWrite> valueText_ = nullptr;

	std::unique_ptr<BaseCamera> camera_ = nullptr;

	Vector3 cameraTranslate = { 0.0f,70.0f,-50.0f };
	Vector3 cameraRotate = { 0.95f,0.0f,0.0f };

	// 当たり判定
	AppCollisionManager* appCollisionManager_ = nullptr;

	// プレイヤー
	std::vector<std::unique_ptr<Player>> players_{};
	std::unique_ptr<Player> preSpawnedPlayer_ = nullptr;

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

	// プレイヤースポーン位置
	std::vector<std::unique_ptr<SpawnPos>> playerSpawn_{};

	//bgm
	std::unique_ptr<Audio> bgm_ = nullptr;

	//ポーズシステム
	std::unique_ptr<PauseSystem> pauseSystem_ = nullptr;

	// プレイヤースポーン位置の数(固定)
	const uint32_t playerSpawnNum_ = 3;
	// プレイヤースポーン(ローテーション用。どのポイントから出現させるか)
	uint32_t playerSpawnIndex_ = 0;
	// プレイヤースポーン位置
	std::vector<Vector3> playerSpawnPositions_{};
	// ローテーション間隔
	const float rotation_ = 240.0f;
	// ローテーション用タイマー
	float rotationTimer_ = rotation_;
	// 出現数上限
	const uint32_t kMaxSpawnNum = 15;
	// 何体出たか
	uint32_t howManyBoogie_ = 0;

	// フィールド上にいるプレイヤーの数
	uint32_t playerNum_ = 0;

	//ゲーム終了フラグ
	bool isGameEnd_ = false;
	// カメラを引くフラグ
	bool isZoomOut_ = false;
	// カメラを寄せるフラグ
	bool isZoomIn_ = false;
	// カメラのイージング経過時間
	float cameraEaseTime_ = 0.0f;
	// カメラのイージング時間
	const float cameraEaseDuration_ = 2.0f;
	// カメラの開始位置
	Vector3 cameraStartPosition_ = { 0.0f,70.0f,-50.0f };
	// カメラの終了位置
	Vector3 cameraEndPosition_ = { 0.0f, 50.0f, -30.0f };
	// 最も近いプレイヤーの位置
	Vector3 nearestPlayerPos_ = {};
	//　最も近いプレイヤーの番号
	uint32_t nearestPlayerNum_ = 0;
	// 最も近い敵の位置
	Vector3 nearestEnemyPos_ = {};
	// 最も近い敵の番号
	uint32_t nearestEnemyNum_ = 0;
	// 最も近い敵の種類
	std::string nearestEnemyType_ = "";
	//　待機時間
	float waitTime_ = 0.0f;
	// 待機時間の長さ
	float waitTimeDuration_ = 0.13f;

	// 攻撃チャージMax
	const float chargeMax_ = 80.0f;
	bool isChargeMax_ = false;
	// 攻撃チャージ
	float charge_ = 0.0f;

	// ゲーム開始のインターバル
	float gameStartDelayTimer_ = 3.0f;
	bool isGameStart_ = false;
	bool hasPreUpdated_ = false;

	Vector2 numSize_ = { 320.0f, 480.0f };
	uint32_t numSizeTimer_ = 0;
};

