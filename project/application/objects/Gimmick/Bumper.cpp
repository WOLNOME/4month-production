#include "Bumper.h"

#include "../../appCollider/AppCollisionManager.h"

void Bumper::Initialize()
{

	worldTransform_.Initialize();
	object_ = std::make_unique<Object3d>();
	object_->InitializeModel("bumper");

	// 当たり判定関係
	appCollisionManager_ = AppCollisionManager::GetInstance();

	objectName_ = "Bumper";
	appCollider_ = std::make_unique<AppCollider>();
	appCollider_->SetOwner(this);
	appCollider_->SetColliderID(objectName_);
	appCollider_->SetShapeData(&aabb_);
	appCollider_->SetShape(AppShape::AppAABB);
	appCollider_->SetAttribute(appCollisionManager_->GetNewAttribute(appCollider_->GetColliderID()));
	appCollider_->SetOnCollision(std::bind(&Bumper::OnCollision, this, std::placeholders::_1));
	appCollisionManager_->RegisterCollider(appCollider_.get());
}

void Bumper::Finalize()
{
	// 各解放処理
	if (appCollider_)
	{
		appCollisionManager_->DeleteCollider(appCollider_.get());
		appCollider_.reset();
	}
}

void Bumper::Update()
{
	worldTransform_.translate_ = position_;
	worldTransform_.scale_ = scale_;

	worldTransform_.UpdateMatrix();

	// 移動処理
	position_ += moveDirection_ * moveSpeed_;
	movedDistance_ += moveSpeed_;

	// 移動範囲を超えたら方向を反転
	if (movedDistance_ >= moveRange_ || movedDistance_ <= -moveRange_)
	{
		moveDirection_ = -moveDirection_;
		movedDistance_ = 0.0f;
	}

	// 当たり判定関係
	aabb_.min = worldTransform_.translate_ - worldTransform_.scale_;
	aabb_.max = worldTransform_.translate_ + worldTransform_.scale_;

	appCollider_->SetPosition(worldTransform_.translate_);
}

void Bumper::Draw(BaseCamera _camera)
{
	object_->Draw(worldTransform_, _camera);
}

void Bumper::OnCollision(const AppCollider* _other)
{
}

void Bumper::SetMoveDirection(const Vector3& direction)
{
	moveDirection_ = direction;
}

void Bumper::SetMoveSpeed(float speed)
{
	moveSpeed_ = speed;
}

void Bumper::SetMoveRange(float range)
{
	moveRange_ = range;
}