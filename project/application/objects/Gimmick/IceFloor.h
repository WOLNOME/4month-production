#pragma once
#include <Object3d.h>
#include <Particle.h>

#include "../GameObject/GameObject.h"
#include "../../appCollider/AppCollider.h"

class IceFloor : public GameObject
{
public:

	~IceFloor() override {};

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw(BaseCamera _camera) override;
	void UpdateTransform() { worldTransform_.UpdateMatrix(); }

private:

	//モデル情報
	WorldTransform worldTransform_;
	std::unique_ptr<Object3d> object_;

	// 当たり判定関係
	AppCollisionManager* appCollisionManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_;
	AppAABB aabb_{};

	//パーティクル
	std::unique_ptr<Particle> particle_;
};

