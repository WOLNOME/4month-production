#include "IceMist.h"
#include <random>

IceMist::~IceMist()
{
	// 当たり判定関係
	appCollisionManager_->DeleteCollider(appCollider_.get());
	appCollider_.reset();
}

void IceMist::Initialize(const std::string& filePath, const Vector3& position, const Vector3& velocity)
{
	// オブジェクトの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->InitializeModel(filePath);
	transform_.Initialize();
    transform_.scale_ = { 1.0f,1.0f,1.0f };
	transform_.translate_ = position;
	transform_.rotate_ = { 0.0f,0.0f,0.0f };
	// 速度ベクトル
	velocity_ = velocity;
	// スタート地点
	startPosition_ = position;
    // 当たり判定関係
    appCollisionManager_ = AppCollisionManager::GetInstance();
    appCollider_ = std::make_unique<AppCollider>();
    appCollider_->SetOwner(this);
    appCollider_->SetColliderID("IceMist");
    appCollider_->SetShapeData(&aabb_);
    appCollider_->SetShape(AppShape::AppAABB);
    appCollider_->SetAttribute(appCollisionManager_->GetNewAttribute(appCollider_->GetColliderID()));
    appCollider_->SetOnCollisionTrigger(std::bind(&IceMist::OnCollisionTrigger, this, std::placeholders::_1));
    appCollider_->SetOnCollision(std::bind(&IceMist::OnCollision, this, std::placeholders::_1));
    appCollisionManager_->RegisterCollider(appCollider_.get());
}

void IceMist::Update()
{
    // 移動
    Move();

    // 行列の更新
    transform_.UpdateMatrix();

    // 当たり判定関係
    UpdateCollider();
}

void IceMist::Draw(BaseCamera camera)
{
	object3d_->Draw(transform_, camera);
}

void IceMist::OnCollision(const AppCollider* other)
{
    
}

void IceMist::OnCollisionTrigger(const AppCollider* other)
{
    if (other->GetColliderID() == "Player")
    {
        //isAlive_ = false;
    }
    else if (other->GetColliderID() == "Obstacle" || other->GetColliderID() == "Bumper")
    {
        isAlive_ = false;
    }
}

void IceMist::Move()
{
	const float deltaTime = 1.0f / 60.0f;

	transform_.translate_ += velocity_ * moveSpeed_ * deltaTime;

	// スタート地点から一定距離離れたら消滅
	if ((transform_.translate_ - startPosition_).Length() > moveRange_)
	{
		isAlive_ = false;
	}
}

void IceMist::UpdateCollider()
{
    // 当たり判定
    aabb_.min = transform_.translate_ - Vector3(0.1f, 0.1f, 0.1f);
    aabb_.max = transform_.translate_ + Vector3(0.1f, 0.1f, 0.1f);
    appCollider_->SetPosition(transform_.translate_);
}
