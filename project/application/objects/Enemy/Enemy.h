#pragma once

#include <Framework.h>
#include <vector>
#include <memory>
#include <MyMath.h>
#include <Object3d.h>

#include "../../appCollider/AppCollider.h"
#include "../GameObject/GameObject.h"

class Player;

class Enemy : public GameObject
{
public:

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;
	

	// 移動
	void Move();

	// 場外処理
	void OutOfField();

	// ImGui
	void ImGuiDraw();

private: // 衝突判定

	void OnCollision(const AppCollider* _other);

public: //セッター

	// カメラをセット
	void SetCamera(BaseCamera* _camera) { camera_.reset(_camera); }

	// プレイヤーの位置との距離をセット
	void SetPlayerPos(const std::vector<std::unique_ptr<Player>>& player);

private:

	// カメラ
	std::unique_ptr<BaseCamera> camera_ = nullptr;

	// モデル情報
	WorldTransform wtEnemy_{};
	std::unique_ptr<Object3d> enemy_ = nullptr;

	// 当たり判定関係
	AppCollisionManager* appCollisionManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_ = nullptr;
	AppAABB aabb_{};
	bool isHit_ = false;
	bool isGround_ = false;

	// 移動速度
	Vector3 moveSpeed_ = { 0.1f,0.0f,0.1f };
	Vector3 moveVel_ = { 0.01f,0.01f,0.01f };
	// 落下速度
	float fallSpeed_ = 0.3f;

	// プレイヤーの位置
	std::vector<Vector3> playerPos_{};
	// プレイヤーとの距離
	Vector3 toPlayer_{};
	// 1フレーム前のプレイヤーとの距離
	Vector3 prevToPlayer_{};
	// 最短距離
	Vector3 minDistance_{};
};
