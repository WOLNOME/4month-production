#include "FreezeEnemy.h"

#include <chrono>

#include "EnemyManager.h"

FreezeEnemy::~FreezeEnemy()
{
	// 当たり判定関係
	appCollisionManager_->DeleteCollider(appCollider_.get());
	appCollider_.reset();
}

FreezeEnemy::FreezeEnemy(EnemyManager* enemyManager)
{
	enemyManager_ = enemyManager;
}

void FreezeEnemy::EnemyInitialize(const std::string& filePath)
{
	// ランダムエンジンの初期化
	randomEngine_.seed(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));
	// オブジェクトの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->InitializeModel(filePath);
	// トランスフォームの初期化
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
	// 凍結攻撃
	FreezeUpdate();
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
	// FreezeEnemyの正面方向を計算
	Vector3 forward = { cos(transform_.rotate_.y), 0.0f, sin(transform_.rotate_.y) };
	forward.Normalize();

	// 扇状に広がる弾を発射
	int numBullets = 3; // 発射する弾の数
	float spreadAngle = 30.0f; // 扇状の角度（度）
	float angleStep = spreadAngle / (numBullets - 1);

	for (int i = 0; i < numBullets; i++)
	{
		// 弾の角度を計算
		float angle = transform_.rotate_.y - spreadAngle / 2.0f + angleStep * i;
		float radian = angle * 3.14159265f / 180.0f;
		Vector3 direction = {
			forward.x * cos(radian) - forward.z * sin(radian),
			0.0f,
			forward.x * sin(radian) + forward.z * cos(radian)
		};
		direction.Normalize();
		// 弾を生成
		enemyManager_->SpawnIceMist(transform_.translate_, direction);
	}
}

void FreezeEnemy::FreezeUpdate()
{// タイマーを更新
	freezeAttackTimer_ += 1.0f / 60.0f;
	if (freezeAttackTimer_ >= freezeAttackInterval_)
	{
		StartFreeze();
		freezeAttackTimer_ = 0.0f;
	}

}

void FreezeEnemy::UpdateCollider()
{
	// 当たり判定
	aabb_.min = transform_.translate_ - transform_.scale_;
	aabb_.max = transform_.translate_ + transform_.scale_;
	appCollider_->SetPosition(transform_.translate_);
}
