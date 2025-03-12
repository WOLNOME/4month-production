#include "FanEnemy.h"

#include <chrono>

#include "EnemyManager.h"

FanEnemy::~FanEnemy()
{
	// 当たり判定関係
	appCollisionManager_->DeleteCollider(appCollider_.get());
	appCollider_.reset();
}

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

	//ランダムエンジンの初期化（シードを現在時刻から取得）
	randomEngine_.seed(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));

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

	// 回転速度の変更
	ChageRotationSpeed();

	// 回転
	transform_.rotate_.y = fmod(transform_.rotate_.y + rotateSpeed_, 2.0f * 3.14159265359f);

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
	if (other->GetColliderID() == "Player")
	{
		isAlive_ = false;
	}
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

void FanEnemy::ChageRotationSpeed()
{
	const float deltaTime = 1.0f / 60.0f;
	rotateSpeedChangeTimer_ += deltaTime;
	if (rotateSpeedChangeTimer_ >= rotateSpeedChangeInterval_)
	{
		// 回転速度をランダムな速度で変更
		std::uniform_real_distribution<float> dis(-0.02f, 0.02f);
		std::uniform_real_distribution<float> disTime(3.0f, 6.0f);
		rotateSpeed_ = dis(randomEngine_);
		rotateSpeedChangeInterval_ = disTime(randomEngine_);
		rotateSpeedChangeTimer_ = 0.0f;
	}
}
