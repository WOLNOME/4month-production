#include "IceFloor.h"

#include "../../engine/appCollider/AppColliderManager.h"
#include "ParticleManager.h"

void IceFloor::Initialize() {

	worldTransform_.Initialize();
	object_ = std::make_unique<Object3d>();
	object_->InitializeModel("iceFloor");

	// 当たり判定関係
	appColliderManager_ = AppColliderManager::GetInstance();

	objectName_ = "IceFloor";
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

	//パーティクル
	auto particleManager = ParticleManager::GetInstance();
	particle_ = std::make_unique<Particle>();
	particle_->Initialize(particleManager->GenerateName("iceFloor"), "cold");
	particle_->emitter_.isGravity = true;
	particle_->emitter_.gravity = 2.0f;
	particle_->emitter_.transform.scale.y = 0.1f;
	particle_->emitter_.transform.scale.x = 3.0f;
	particle_->emitter_.transform.scale.z = 3.0f;
}

void IceFloor::Finalize() {
	// 各解放処理
	if (appCollider_) {
		appColliderManager_->DeleteCollider(appCollider_.get());
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
