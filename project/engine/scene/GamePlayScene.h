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
#include "../../application/objects/Enemy/Enemy.h"
#include "../../application/objects/Field/Field.h"
#include "../../application/appCollider/AppCollisionManager.h"

class GamePlayScene : public BaseScene
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

	// ImGui
	void ImGuiDraw();

private://メンバ変数

	Input* input_ = nullptr;

	std::unique_ptr<BaseCamera> camera_ = nullptr;

	Vector3 cameraTranslate = { 0.0f,25.0f,-32.0f };
	Vector3 cameraRotate = { 0.65f,0.0f,0.0f };

	// 当たり判定
	AppCollisionManager* appCollisionManager_ = nullptr;

	// プレイヤー
	std::list<std::unique_ptr<Player>> players_{};

	// エネミー
	std::unique_ptr<Enemy> enemy_ = nullptr;

	// フィールド
	std::unique_ptr<Field> field_ = nullptr;
	// プレイヤースポーン位置
	Vector3 playerSpawnPos_ = { 0.0f,13.0f,2.0f };

};

