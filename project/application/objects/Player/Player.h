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
	void Draw(BaseCamera _camera) override;
	
	// 移動
	void Move();
	
	void MovePosition();

	// 場外処理
	void OutOfField();
	
	// 攻撃
	void Attack();

	// ImGui
	void ImGuiDraw();


private: // 衝突判定

	// 当たっている間ずっと呼ばれる
	void OnCollision(const AppCollider* _other);

	// 当たった瞬間だけ呼ばれる
	void OnCollisionTrigger(const AppCollider* _other);

public: // ゲッター
	
	// 死亡フラグを取得
	bool IsDead() const { return isDead_; }

public: //セッター

	// プレイヤーの位置をセット
	void SetPlayerPos(const Vector3& _pos) { wtPlayer_.translate_ = _pos; }

private:

	// 入力
	Input* input_ = nullptr;

	// プレーヤーモデル情報
	WorldTransform wtPlayer_{};
	std::unique_ptr<Object3d> player_ = nullptr;

	// 当たり判定関係
	AppCollisionManager* appCollisionManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_ = nullptr;
	AppAABB aabb_{};
	bool isHit_ = false;
	bool isGround_ = false;
	// エネミーの位置
	Vector3 enemyPosition_{};
	// ノックバックの時間
	float knockBackTime_ = 0.0f;


	// 移動速度
	Vector3 moveSpeed_ = { 0.1f,0.0f,0.1f };
	Vector3 moveVel_ = { 0.01f,0.01f,0.01f };
	// 落下速度
	float fallSpeed_ = 0.3f;
	// 摩擦係数（減速率）
	float attackFriction_ = 2.0f;
	float friction_ = 0.01f;

	// 攻撃時間
	const float attackTime_ = 12;
	float attackTimeCounter_ = attackTime_;

	Vector3 attackToEnemy_{};

	// 死亡フラグ
	bool isDead_ = false;

};

