#pragma once
#include "Object3d.h"
#include "Vector3.h"
#include "WorldTransform.h"
#include "application/appCollider/AppCollisionManager.h"

class Wind : public GameObject
{
public:
	~Wind();
	void Initialize(const std::string& filePath, const Vector3& position, const Vector3& direction);
	void Update()override;
	void Draw(BaseCamera camera) override;
	// 当たっている間ずっと呼ばれる
	void OnCollision(const AppCollider* other);
	// 当たった瞬間だけ呼ばれる
	void OnCollisionTrigger(const AppCollider* other);

	const Vector3& GetDirection() const { return direction_; }
	void SetDirection(const Vector3& direction) { direction_ = direction; }
	Vector3 GetPosition() const { return transform_.translate_; }
	void SetPosition(const Vector3& position) { transform_.translate_ = position; }
	bool IsAlive() const { return isAlive_; }
private:
	void Initialize() override {};
	void Finalize() override {};

private:
	WorldTransform transform_;
	int32_t textureHandle_ = EOF;
	std::unique_ptr<Object3d> object3d_;
	//当たり判定
	AppCollisionManager* appCollisionManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_ = nullptr;
	AppAABB aabb_{};

	// 風向き
    Vector3 direction_;
	// 速度
	float speed_ = 20.0f;
	// 移動できる範囲
	float range_ = 10.0f;
	// 開始位置
	Vector3 startPosition_;
	// 生存フラグ
	bool isAlive_ = true;
};
