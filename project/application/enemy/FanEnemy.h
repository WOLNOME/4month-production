#pragma once
#include "BaseEnemy.h"
#include "../appCollider/AppCollisionManager.h"
#include "application/objects/GameObject/GameObject.h"

class EnemyManager;

class FanEnemy : public BaseEnemy, public GameObject
{
public:
	void EnemyInitialize(const std::string& filePath) override;
	void EnemyUpdate() override;
	void EnemyDraw(const BaseCamera& camera) override;
	void OnCollision(const AppCollider* _other);
	void OnCollisionTrigger(const AppCollider* other);

	void Initialize() override {}
	void Update() override{}
	void Draw(BaseCamera camera) override { camera; }
	void Finalize() override{}

	void SetPosition(const Vector3& position) { transform_.translate_ = position; }
	Vector3 GetPosition() const { return transform_.translate_; }
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }

private:
	void Move();
	void StartFan();
	void FanUpdate();

private:
	//速度ベクトル
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	//摩擦係数
	const float friction_ = 2.0f;
	float rotateSpeed_ = 0.02f;
	//風を生成する間隔
	float windSpawnInterval_ = 0.2f;
	//風を生成するタイマー
	float windSpawnTimer_ = 0.0f;
	//エネミーマネージャーのポインタ
	EnemyManager* enemyManager_ = nullptr;
	//当たり判定
	AppCollisionManager* appCollisionManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_ = nullptr;
	AppAABB aabb_;
};

