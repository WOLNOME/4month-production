#pragma once
#include <random>
#include <Particle.h>

#include "BaseEnemy.h"
#include "../appCollider/AppCollisionManager.h"
#include "application/objects/GameObject/GameObject.h"

class EnemyManager;

class FreezeEnemy : public BaseEnemy, public GameObject
{
public:
	~FreezeEnemy();
	FreezeEnemy(EnemyManager* enemyManager);
	void EnemyInitialize(const std::string& filePath) override;
	void EnemyUpdate() override;
	void EnemyDraw(const BaseCamera& camera) override;
	void OnCollision(const AppCollider* other);
	void OnCollisionTrigger(const AppCollider* other);

	bool IsAlive() const { return isAlive_; }
	bool IsGround() const { return isGround_; }
	void SetPosition(const Vector3& position) { transform_.translate_ = position; }
	Vector3 GetPosition() const { return transform_.translate_; }
	Vector3 GetRotate() const { return transform_.rotate_; }
	void SetRotate(const Vector3& rotate) { transform_.rotate_ = rotate; }
	void SetTargetPosition(const Vector3& targetPosition) { targetPosition_ = targetPosition; }
	void UpdateTransform() { transform_.UpdateMatrix(); }
	void SetMoveable(bool moveable) { isMoveable_ = moveable; }
	bool IsPlayingDeadEffect() const { return deadEffect_->emitter_.isPlay; }
	bool isDeadSEPlayed_ = false;

	void Initialize() override {}
	void Update() override {}
	void Draw(BaseCamera _camera) override { _camera; }
	void Finalize() override {}
private:
	void Move();
	void OutOfField();
	void StartFreeze();
	void FreezeUpdate();
	void UpdateCollider();

	// 障害物にぶつかったとき、最小の移動量で押し戻すベクトルを求める
	Vector3 ComputePenetration(const AppAABB& otherAABB);

	//氷の上の移動
	void MoveOnIce();

private:
	//パーティクル
	std::unique_ptr<Particle> deadEffect_ = nullptr;
	int countDeadEffect_;

	//速度ベクトル
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	//摩擦係数
	const float friction_ = 2.0f;
	//地面にいるか
	bool isGround_ = true;
	//エネミーマネージャー
	EnemyManager* enemyManager_ = nullptr;
	//当たり判定
	AppCollisionManager* appCollisionManager_ = nullptr;
	//当たり判定
	std::unique_ptr<AppCollider> appCollider_ = nullptr;
	//当たり判定の形状
	AppAABB aabb_;
	//生存フラグ
	bool isAlive_ = true;
	//ランダムエンジン
	std::mt19937 randomEngine_;
	//攻撃対象の座標
	Vector3 targetPosition_ = {};
	//凍結攻撃を行う間隔
	float freezeAttackInterval_ = 0.4f;
	//凍結攻撃を行うタイマー
	float freezeAttackTimer_ = 0.0f;

	//氷の上にいるときの摩擦係数
	float frictionOnIce_ = 0.995f;

	// 行動不能フラグ
	bool isMoveable_ = true;
};
