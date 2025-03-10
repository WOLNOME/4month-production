#pragma once
#include "BaseEnemy.h"
#include "../appCollider/AppCollisionManager.h"
#include "application/objects/GameObject/GameObject.h"

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

private:
	void Move();
	void StartFan();

private:
	//速度ベクトル
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	//摩擦係数
	const float friction_ = 2.0f;
	//ファンの届く範囲
	float range_ = 5.0f;
	//ファンの向き
	Vector3 direction_ = { 0.0f,0.0f,0.0f };

	//当たり判定
	AppCollisionManager* appCollisionManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_ = nullptr;
	AppAABB aabb_;
};

