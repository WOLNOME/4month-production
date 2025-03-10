#include "Wind.h"

void Wind::Initialize(const std::string& filePath, const Vector3& position, const Vector3& direction)
{
	object3d_ = std::make_unique<Object3d>();
	object3d_->InitializeModel(filePath);
	transform_.Initialize();
	transform_.translate_ = position;
	transform_.scale_ = { 1.0f, 1.0f, 1.0f };
	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };
	direction_ = direction;
}

void Wind::Update()
{
	// 移動
	const float deltaTime = 1.0f / 60.0f;
	transform_.translate_ += direction_ * deltaTime;

	// 行列の更新
	transform_.UpdateMatrix();
}

void Wind::Draw(const BaseCamera& camera)
{
	object3d_->Draw(transform_, camera);
}
