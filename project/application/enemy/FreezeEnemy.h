#pragma once
#include "BaseEnemy.h"
#include "../appCollider/AppCollisionManager.h"
#include "application/objects/GameObject/GameObject.h"

class FreezeEnemy : public BaseEnemy, public GameObject
{
public:
	~FreezeEnemy();
	void EnemyInitialize(const std::string& filePath) override;
	void EnemyUpdate() override;
	void EnemyDraw(const BaseCamera& camera) override;
	void OnCollision(const AppCollider* other);
	void OnCollisionTrigger(const AppCollider* other);

	bool IsAlive() const { return isAlive_; }
	void SetPosition(const Vector3& position) { transform_.translate_ = position; }
	Vector3 GetPosition() const { return transform_.translate_; }

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

private:
	//速度ベクトル
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	//摩擦係数
	const float friction_ = 2.0f;
	//地面にいるか
	bool isGround_ = true;
	//当たり判定
	AppCollisionManager* appCollisionManager_ = nullptr;
	//当たり判定
	std::unique_ptr<AppCollider> appCollider_ = nullptr;
	//当たり判定の形状
	AppAABB aabb_;
	//生存フラグ
	bool isAlive_ = true;
};