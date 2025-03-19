#include "FreezeEnemy.h"

FreezeEnemy::~FreezeEnemy()
{
	// 当たり判定関係
	appCollisionManager_->DeleteCollider(appCollider_.get());
	appCollider_.reset();
}

void FreezeEnemy::EnemyInitialize(const std::string& filePath)
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
	objectName_ = "FreezeEnemy";
	appCollider_ = std::make_unique<AppCollider>();
	appCollider_->SetOwner(this);
	appCollider_->SetColliderID(objectName_);
	appCollider_->SetShapeData(&aabb_);
	appCollider_->SetShape(AppShape::AppAABB);
	appCollider_->SetAttribute(appCollisionManager_->GetNewAttribute(appCollider_->GetColliderID()));
	appCollider_->SetOnCollisionTrigger(std::bind(&FreezeEnemy::OnCollisionTrigger, this, std::placeholders::_1));
	appCollider_->SetOnCollision(std::bind(&FreezeEnemy::OnCollision, this, std::placeholders::_1));
	appCollisionManager_->RegisterCollider(appCollider_.get());
}

void FreezeEnemy::EnemyUpdate()
{
	// 移動
	Move();
	// 場外処理
	OutOfField();
	//行列の更新
	transform_.UpdateMatrix();
	// 当たり判定関係
	UpdateCollider();
}

void FreezeEnemy::EnemyDraw(const BaseCamera& camera)
{
	object3d_->Draw(transform_, camera);
}

void FreezeEnemy::OnCollision(const AppCollider* other)
{
	if (other->GetColliderID() == "Field")
	{
		// 地面にいる
		isGround_ = true;
	}

	if (other->GetColliderID() == "Player")
	{
		// プレイヤーの位置
		Vector3 playerPosition = other->GetOwner()->GetPosition();

		// プレイヤーの位置から逃げる
		Vector3 runDirection = transform_.translate_ - playerPosition;

		// ノックバック
		velocity_ += runDirection * 0.5f;
		velocity_.y = 0.0f;
	}
}

void FreezeEnemy::OnCollisionTrigger(const AppCollider* other)
{
	
}

void FreezeEnemy::Move()
{
	const float deltaTime = 1.0f / 60.0f;

	// 摩擦
	Vector3 friction = -velocity_ * friction_ * deltaTime;
	velocity_ += friction;

	// 速度が小さくなったら停止
	if (velocity_.Length() < 0.01f)
	{
		velocity_ = { 0.0f,0.0f,0.0f };
	}
	
	// 移動
	transform_.translate_ += velocity_ * deltaTime;
}

void FreezeEnemy::OutOfField()
{
	// 落下処理
	float fallSpeed = 0.3f;

	if (isGround_ == false)
	{
		transform_.translate_.y -= fallSpeed;
	}	

	if (transform_.translate_.y < -10.0f)
	{
		isAlive_ = false;
	}

	isGround_ = false;
}

void FreezeEnemy::StartFreeze()
{
	// 凍結処理

}

void FreezeEnemy::UpdateCollider()
{
	// 当たり判定
	aabb_.min = transform_.translate_ - transform_.scale_;
	aabb_.max = transform_.translate_ + transform_.scale_;
	appCollider_->SetPosition(transform_.translate_);
}
