#pragma once
#include <Object3d.h>

#include "../GameObject/GameObject.h"
#include "../../appCollider/AppCollider.h"

class Obstacle : public GameObject
{
public:

	~Obstacle() override {};

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw(BaseCamera _camera) override;

private:

	//モデル情報
	WorldTransform worldTransform_;
	std::unique_ptr<Object3d> object_;

	// 当たり判定関係
	AppCollisionManager* appCollisionManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_;
	AppAABB aabb_{};
};

