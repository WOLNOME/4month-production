#pragma once
#include "BaseEnemy.h"
#include "../appCollider/AppCollisionManager.h"
#include "application/objects/GameObject/GameObject.h"
#include <memory>

class IceMist : public GameObject
{
public:
    ~IceMist();
	void Initialize(const std::string& filePath, const Vector3& position, const Vector3& velocity);
    void Update();
    void Draw(const BaseCamera& camera);
    void OnCollision(const AppCollider* other);
    void OnCollisionTrigger(const AppCollider* other);

    bool IsAlive() const { return isAlive_; }

private:
    void Move();
    void UpdateCollider();

private:
	//オブジェクト
	std::unique_ptr<Object3d> object3d_ = nullptr;
    //速度ベクトル
    Vector3 velocity_;
    //摩擦係数
    const float friction_ = 0.1f;
    //生存フラグ
    bool isAlive_ = true;
    //当たり判定
    AppCollisionManager* appCollisionManager_ = nullptr;
    std::unique_ptr<AppCollider> appCollider_ = nullptr;
    AppAABB aabb_;
    //トランスフォーム
    WorldTransform transform_;
};
