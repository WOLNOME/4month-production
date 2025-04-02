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

class GamePlayScene3 : public BaseScene
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 終了時
	/// </summary>
	void Finalize() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;
	/// <summary>
	/// テキスト描画
	/// </summary>
	void TextDraw() override;

	// ImGui
	void ImGuiDraw();
	
	// プレイヤースポーン処理
	void playerSpawnRotation();

	// Player攻撃チャージ
	void playerTackleCharge();

private://メンバ変数

	Input* input_ = nullptr;

	// プライト
	uint32_t textureHandleUI_PLAY_ = 0u;
	std::unique_ptr<Sprite> spriteUI_PLAY_ = nullptr;

	std::unique_ptr<BaseCamera> camera_ = nullptr;

	Vector3 cameraTranslate = { 0.0f,100.0f,-75.0f };
	Vector3 cameraRotate = { 0.95f,0.0f,0.0f };

	// 当たり判定
	AppCollisionManager* appCollisionManager_ = nullptr;

	// プレイヤー
	std::vector<std::unique_ptr<Player>> players_{};

	//エネミーマネージャー
	std::unique_ptr<EnemyManager> enemyManager_;

	//スカイドーム
	std::unique_ptr<Skydome> skydome_ = nullptr;
	// フィールド
	std::unique_ptr<Field> field_ = nullptr;

	//氷の床
	std::vector<std::unique_ptr<IceFloor>> icefloors_;

	// プレイヤースポーン位置
	std::vector<std::unique_ptr<SpawnPos>> playerSpawn_{};

	// プレイヤースポーン位置の数(固定)
	const uint32_t playerSpawnNum_ = 3;
	// プレイヤースポーン(ローテーション用)
	uint32_t playerSpawnIndex_ = 0;
	// プレイヤースポーン位置
	std::vector<Vector3> playerSpawnPositions_{};
	// ローテーション間隔
	const float rotation_ = 120.0f;
	// ローテーション用タイマー
	float rotationTimer_ = rotation_;
	// 何体出たか
	uint32_t howManyBoogie_ = 0;

	// フィールド上にいるプレイヤーの数
	uint32_t playerNum_ = 0;


	// 攻撃チャージMax
	const float chargeMax_ = 80.0f;
	// 攻撃チャージ
	float charge_ = 0.0f;

};

