#pragma once
#include "BaseEnemy.h"
#include "application/appCollider/AppCollisionManager.h"
#include "application/objects/GameObject/GameObject.h"

class FanEnemy : public BaseEnemy, public GameObject
{
public:
	void EnemyInitialize(const std::string& filePath) override;
	void EnemyUpdate() override;
	void EnemyDraw(const BaseCamera& camera) override;
	void OnCollision(const AppCollider* _other);
	void OnCollisionTrigger(const AppCollider* _other);

private:
	AppCollisionManager* appCollisionManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_ = nullptr;
	AppAABB aabb_;
};

