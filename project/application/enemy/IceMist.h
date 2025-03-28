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
	void Update() override;
	void Draw(BaseCamera camera) override;
    void OnCollision(const AppCollider* other);
    void OnCollisionTrigger(const AppCollider* other);

    bool IsAlive() const { return isAlive_; }

    void Initialize() override {};
    
	void Finalize() override {};

private:
    void Move();
    void UpdateCollider();

private:
	//オブジェクト
	std::unique_ptr<Object3d> object3d_ = nullptr;
    //速度ベクトル
    Vector3 velocity_;
    //摩擦係数
    const float friction_ = 0.4f;
    //スタート地点
	Vector3 startPosition_;
    //移動範囲
	float moveRange_ = 10.0f;
    //移動速度
	float moveSpeed_ = 7.0f;
    //生存フラグ
    bool isAlive_ = true;
    //当たり判定
    AppCollisionManager* appCollisionManager_ = nullptr;
    std::unique_ptr<AppCollider> appCollider_ = nullptr;
    AppAABB aabb_;
    //トランスフォーム
    WorldTransform transform_;
};
