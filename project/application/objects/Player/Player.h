#pragma once

#include "Input.h"

#include <Framework.h>
#include <vector>
#include <memory>
#include <MyMath.h>
#include <Object3d.h>

#include "../../appCollider/AppCollider.h"
#include "../GameObject/GameObject.h"

class Player : public GameObject
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
	
	// 攻撃
	void Attack();

	// ImGui
	void ImGuiDraw();

private: // 衝突判定

	void OnCollision(const AppCollider* _other);

public: //セッター

	// カメラをセット
	void SetCamera(BaseCamera* _camera) { camera_.reset(_camera); }

private:

	// 入力
	Input* input_ = nullptr;

	// カメラ
	std::unique_ptr<BaseCamera> camera_ = nullptr;

	// モデル情報
	WorldTransform wtPlayer_{};
	std::unique_ptr<Object3d> player_ = nullptr;

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

	// 攻撃
	bool isAttack_ = false;
	// 攻撃時間
	const float attackTime_ = 12;
	float attackTimeCounter_ = attackTime_;

};

