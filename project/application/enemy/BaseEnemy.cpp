#include "BaseEnemy.h"

#include "ParticleManager.h"

void BaseEnemy::EnemyInitialize(const std::string& filePath)
{
	//オブジェクトの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->InitializeModel(filePath);

	//トランスフォームの初期化
	transform_.Initialize();
	transform_.scale_ = { 1.0f, 1.0f, 1.0f };
	transform_.translate_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };
	// 当たり判定関係
	appCollisionManager_ = AppCollisionManager::GetInstance();
	appCollider_ = std::make_unique<AppCollider>();
	appCollider_->SetOwner(this);
	appCollider_->SetShapeData(&aabb_);
	appCollider_->SetShape(AppShape::AppAABB);
	appCollisionManager_->RegisterCollider(appCollider_.get());

	// パーティクル
	auto particleManager = ParticleManager::GetInstance();
	deadEffect_ = std::make_unique<Particle>();
	deadEffect_->Initialize(particleManager->GenerateName("deadFreezeEnemy"), "deadEnemy");
	deadEffect_->emitter_.isGravity = true;
	deadEffect_->emitter_.gravity = -150.0f;
	deadEffect_->emitter_.isPlay = false;
	countDeadEffect_ = 0;

	fallSE_ = std::make_unique<Audio>();
	fallSE_->Initialize("soundeffect/fall.wav");
}

void BaseEnemy::Finalize()
{
	// 当たり判定の解放
	if (appCollider_)
	{
		appCollisionManager_->DeleteCollider(appCollider_.get());
		appCollider_.reset();
	}
}
