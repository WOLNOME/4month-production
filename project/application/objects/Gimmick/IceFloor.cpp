#include "IceFloor.h"

#include "../../appCollider/AppCollisionManager.h"
#include "RandomStringUtil.h"

void IceFloor::Initialize() {

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

	particle_ = std::make_unique<Particle>();
	particle_->Initialize("iceFloor" + RandomStringUtil::GenerateRandomString(3), "cold");
	particle_->emitter_.isGravity = true;
	particle_->emitter_.gravity = 2.0f;
	particle_->emitter_.transform.scale.y = 0.1f;
	particle_->emitter_.transform.scale.x = 3.0f;
	particle_->emitter_.transform.scale.z = 3.0f;
}

void IceFloor::Finalize() {
	// 各解放処理
	if (appCollider_) {
		appCollisionManager_->DeleteCollider(appCollider_.get());
		appCollider_.reset();
	}
}

void IceFloor::Update() {
	worldTransform_.translate_ = position_;
	worldTransform_.scale_ = scale_;
	worldTransform_.rotate_ = rotation_;

	worldTransform_.UpdateMatrix();

	// 当たり判定関係
	aabb_.min = worldTransform_.translate_ - worldTransform_.scale_;
	aabb_.max = worldTransform_.translate_ + worldTransform_.scale_;

	appCollider_->SetPosition(worldTransform_.translate_);

	particle_->emitter_.transform.translate = worldTransform_.translate_;
	particle_->emitter_.transform.translate.y += 0.5f;
}

void IceFloor::Draw(BaseCamera _camera) {
	object_->Draw(worldTransform_, _camera);
}
