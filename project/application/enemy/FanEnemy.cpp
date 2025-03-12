#include "FanEnemy.h"
#include "EnemyManager.h"

void FanEnemy::EnemyInitialize(const std::string& filePath)
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
	objectName_ = "FanEnemy";
	appCollider_ = std::make_unique<AppCollider>();
	appCollider_->SetOwner(this);
	appCollider_->SetColliderID(objectName_);
	appCollider_->SetShapeData(&aabb_);
	appCollider_->SetShape(AppShape::AppAABB);
	appCollider_->SetAttribute(appCollisionManager_->GetNewAttribute(appCollider_->GetColliderID()));
	appCollider_->SetOnCollisionTrigger(std::bind(&FanEnemy::OnCollisionTrigger, this, std::placeholders::_1));
	appCollider_->SetOnCollision(std::bind(&FanEnemy::OnCollision, this, std::placeholders::_1));
	appCollisionManager_->RegisterCollider(appCollider_.get());
}

void FanEnemy::EnemyUpdate()
{
	// 移動
	Move();

	// 回転
	transform_.rotate_.y += rotateSpeed_;
	if (transform_.rotate_.y >= 3.14f)
	{
		transform_.rotate_.y = -3.14f;
	}

	// 風の更新
	FanUpdate();

	//行列の更新
	transform_.UpdateMatrix();

	// 当たり判定関係
	aabb_.min = transform_.translate_ - transform_.scale_;
	aabb_.max = transform_.translate_ + transform_.scale_;
	appCollider_->SetPosition(transform_.translate_);
}

void FanEnemy::EnemyDraw(const BaseCamera& camera)
{
	object3d_->Draw(transform_, camera);
}

void FanEnemy::OnCollision(const AppCollider* _other)
{

}

void FanEnemy::OnCollisionTrigger(const AppCollider* other)
{
}

void FanEnemy::Move()
{
 	const float deltaTime = 1.0f / 60.0f;

	// 摩擦処理
	Vector3 friction = -velocity_ * friction_ * deltaTime;
	velocity_ += friction;

	// 速度が小さくなったら停止
	if (velocity_.Length() < 0.1f)
	{
		velocity_ = { 0.0f,0.0f,0.0f };
	}
	
	// 移動処理
	transform_.translate_ += velocity_ * deltaTime;
}

void FanEnemy::StartFan()
{
	// 風の方向を設定
	Vector3 direction = { cos(transform_.rotate_.y), 0.0f, sin(transform_.rotate_.y) };
	direction.Normalize();

	// 風を生成
	enemyManager_->SpawnWind(transform_.translate_, direction);
}

void FanEnemy::FanUpdate()
{
	// 風の生成タイマーを更新
	windSpawnTimer_ += 1.0f / 60.0f;
	if (windSpawnTimer_ >= windSpawnInterval_)
	{
		StartFan();
		windSpawnTimer_ = 0.0f;
	}
}
