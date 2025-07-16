#include "Obstacle.h"

#include "../../engine/appCollider/AppColliderManager.h"

void Obstacle::Initialize()
{

	worldTransform_.Initialize();
	object_ = std::make_unique<Object3d>();
	object_->InitializeModel("obstacle");

	// 当たり判定関係
	appColliderManager_ = AppColliderManager::GetInstance();

	objectName_ = "Obstacle";
	appCollider_ = std::make_unique<AppCollider>();
	desc =
	{
		//ここに設定
		.owner = this,
		.colliderID = objectName_,
		.shape = AppShape::AppAABB,
		.shapeData = &aabb_,
		.attribute = appColliderManager_->GetNewAttribute(objectName_),
	};
	appCollider_->MakeAABBDesc(desc);
	appColliderManager_->RegisterCollider(appCollider_.get());
}

void Obstacle::Finalize()
{
	// 各解放処理
	if (appCollider_)
	{
		appColliderManager_->DeleteCollider(appCollider_.get());
		appCollider_.reset();
	}
}

void Obstacle::Update()
{
	worldTransform_.translate_ = position_;
	worldTransform_.scale_ = scale_;

	worldTransform_.UpdateMatrix();

	// 当たり判定関係
	aabb_.min = worldTransform_.translate_ - worldTransform_.scale_;
	aabb_.max = worldTransform_.translate_ + worldTransform_.scale_;

	appCollider_->SetPosition(worldTransform_.translate_);
}

void Obstacle::Draw(BaseCamera _camera)
{
	object_->Draw(worldTransform_, _camera);
}