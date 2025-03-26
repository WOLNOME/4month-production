#include "IceFloor.h"

#include "../../appCollider/AppCollisionManager.h"

void IceFloor::Initialize()
{

	worldTransform_.Initialize();
	object_ = std::make_unique<Object3d>();
	object_->InitializeModel("iceFloor");

	// 当たり判定関係
	appCollisionManager_ = AppCollisionManager::GetInstance();

	objectName_ = "IceFloor";
	appCollider_ = std::make_unique<AppCollider>();
	appCollider_->SetOwner(this);
	appCollider_->SetColliderID(objectName_);
	appCollider_->SetShapeData(&aabb_);
	appCollider_->SetShape(AppShape::AppAABB);
	appCollider_->SetAttribute(appCollisionManager_->GetNewAttribute(appCollider_->GetColliderID()));
	appCollisionManager_->RegisterCollider(appCollider_.get());
}

void IceFloor::Finalize()
{
	// 各解放処理
	if (appCollider_)
	{
		appCollisionManager_->DeleteCollider(appCollider_.get());
		appCollider_.reset();
	}
}

void IceFloor::Update()
{
	worldTransform_.translate_ = position_;
	worldTransform_.scale_ = scale_;
	worldTransform_.rotate_ = rotation_;

	worldTransform_.UpdateMatrix();

	// 当たり判定関係
	aabb_.min = worldTransform_.translate_ - worldTransform_.scale_;
	aabb_.max = worldTransform_.translate_ + worldTransform_.scale_;

	appCollider_->SetPosition(worldTransform_.translate_);
}

void IceFloor::Draw(BaseCamera _camera)
{
	object_->Draw(worldTransform_, _camera);
}
