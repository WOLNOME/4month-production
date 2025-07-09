#pragma once
#include <random>
#include <Particle.h>

#include "BaseEnemy.h"
#include "../appCollider/AppCollisionManager.h"
#include "application/objects/GameObject/GameObject.h"

class EnemyManager;

class FreezeEnemy : public BaseEnemy
{
public:
	~FreezeEnemy();
	FreezeEnemy(EnemyManager* enemyManager);
	void EnemyInitialize(const std::string& filePath) override;
	void EnemyUpdate() override;
	void EnemyDraw(const BaseCamera& camera) override;
	void OnCollision(const AppCollider* other);
	void OnCollisionTrigger(const AppCollider* other);

	void Initialize() override {}
	void Update() override {}
	void Draw(BaseCamera _camera) override { _camera; }

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
	//速度ベクトル
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	//摩擦係数
	const float friction_ = 2.0f;
	//ランダムエンジン
	std::mt19937 randomEngine_;
	//凍結攻撃を行う間隔
	float freezeAttackInterval_ = 0.4f;
	//凍結攻撃を行うタイマー
	float freezeAttackTimer_ = 0.0f;

	//氷の上にいるときの摩擦係数
	float frictionOnIce_ = 0.995f;
};
