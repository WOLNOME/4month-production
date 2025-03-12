#pragma once
#include "Object3d.h"
#include "Vector3.h"
#include "WorldTransform.h"

class Wind {
public:
	void Initialize(const std::string& filePath, const Vector3& position, const Vector3& direction);
	void Update();
	void Draw(const BaseCamera& camera);

    const Vector3& GetDirection() const { return direction_; }
    void SetDirection(const Vector3& direction) { direction_ = direction; }
	const Vector3& GetPosition() const { return transform_.translate_; }
	void SetPosition(const Vector3& position) { transform_.translate_ = position; }
	bool IsAlive() const { return isAlive_; }

private:
	WorldTransform transform_;
	std::unique_ptr<Object3d> object3d_;
	// 風向き
    Vector3 direction_;
	// 速度
	float speed_ = 10.0f;
	// 移動できる範囲
	float range_ = 10.0f;
	// 開始位置
	Vector3 startPosition_;
	// 生存フラグ
	bool isAlive_ = true;
};
