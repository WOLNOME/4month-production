#pragma once
#include <Object3d.h>

#include "../GameObject/GameObject.h"
#include "../../appCollider/AppCollider.h"

class Bumper : public GameObject
{
public:

	~Bumper() override {};

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw(BaseCamera _camera) override;
	void OnCollision(const AppCollider* _other);

	// 移動方向と速度を設定するメソッドを追加
	void SetMoveDirection(const Vector3& direction);
	void SetMoveSpeed(float speed);
	void SetMoveRange(float range);



private:

	//モデル情報
	WorldTransform worldTransform_;
	std::unique_ptr<Object3d> object_;

	// 当たり判定関係
	AppCollisionManager* appCollisionManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_;
	AppAABB aabb_{};

	// 移動方向と速度のプロパティを追加
	Vector3 moveDirection_ = { 0.0f, 0.0f, 0.0f };
	float moveSpeed_ = 0.0f;
	float moveRange_ = 0.0f;
	float movedDistance_ = 0.0f;
};