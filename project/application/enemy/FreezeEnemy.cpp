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
	//氷の上にいるとき
	if (onIce_)
	{
		MoveOnIce();
	}
	else
	{
		// 移動
		Move();
	}
	// 凍結攻撃
	FreezeUpdate();
	// 場外処理
	OutOfField();
	//行列の更新
	transform_.UpdateMatrix();
	// 当たり判定関係
	UpdateCollider();

	onIce_ = false;
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
	else if (other->GetColliderID() == "Obstacle")
	{
		transform_.translate_ += ComputePenetration(*other->GetAABB());
		//行列の更新
		transform_.UpdateMatrix();

		// 当たり判定関係
		aabb_.min = transform_.translate_ - transform_.scale_;
		aabb_.max = transform_.translate_ + transform_.scale_;
		appCollider_->SetPosition(transform_.translate_);
	}
	else if (other->GetColliderID() == "Bumper")
	{
		Vector3 penetration = ComputePenetration(*other->GetAABB());
		transform_.translate_ += penetration;
		penetration.Normalize();
		// ノックバック
		velocity_ = penetration;
		velocity_ *= 20.0f;
		velocity_.y = 0.0f;

	}
	else if (other->GetColliderID() == "IceFloor")
	{
		onIce_ = true;
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

	//敵同士の当たり判定
	if (other->GetColliderID() == "FreezeEnemy" || other->GetColliderID() == "TackleEnemy" || other->GetColliderID() == "FanEnemy")
	{
		// 敵の位置
		Vector3 enemyPosition = other->GetOwner()->GetPosition();

		// 敵同士が重ならないようにする
		Vector3 direction = transform_.translate_ - enemyPosition;
		direction.Normalize();
		float distance = 2.5f; // 敵同士の間の距離を調整するための値

		// 互いに重ならないように少しずつ位置を調整
		if ((transform_.translate_ - enemyPosition).Length() < distance)
		{
			transform_.translate_ += direction * 0.1f; // 微調整のための値
			transform_.translate_.y = 1.0f;
		}
	}
}

void FreezeEnemy::OnCollisionTrigger(const AppCollider* other)
{
	if (other->GetColliderID() == "Player" && other->GetOwner()->IsAttack())
	{
		// プレイヤーの位置
		Vector3 playerPosition = other->GetOwner()->GetPosition();

		// プレイヤーの位置から逃げる
		Vector3 runDirection = transform_.translate_ - playerPosition;

		// ノックバック
		velocity_ = runDirection;
		velocity_ *= 20.0f;
		velocity_.y = 0.0f;
	}
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
	// targetPositionの方向を計算
	Vector3 directionToTarget = targetPosition_ - transform_.translate_;
	directionToTarget.Normalize();

	// 扇状に広がる弾を発射
	int numBullets = 3; // 発射する弾の数
	float spreadAngle = 30.0f; // 扇状の角度（度）
	float angleStep = spreadAngle / (numBullets - 1);

	for (int i = 0; i < numBullets; i++)
	{
		// 弾の角度を計算
		float angle = -spreadAngle / 2.0f + angleStep * i;
		float radian = angle * 3.14159265f / 180.0f;
		Vector3 direction = {
			directionToTarget.x * cos(radian) - directionToTarget.z * sin(radian),
			0.0f,
			directionToTarget.x * sin(radian) + directionToTarget.z * cos(radian)
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

Vector3 FreezeEnemy::ComputePenetration(const AppAABB& otherAABB)
{
	Vector3 penetration;

	//X軸方向に押し戻すベクトル
	float overlapX1 = otherAABB.max.x - aabb_.min.x;
	float overlapX2 = aabb_.max.x - otherAABB.min.x;
	float penetrationX = (overlapX1 < overlapX2) ? overlapX1 : -overlapX2;

	//Z軸方向に押し戻すベクトル
	float overlapZ1 = otherAABB.max.z - aabb_.min.z;
	float overlapZ2 = aabb_.max.z - otherAABB.min.z;
	float penetrationZ = (overlapZ1 < overlapZ2) ? overlapZ1 : -overlapZ2;

	//ベクトルの絶対値を求める
	float absX = std::abs(penetrationX);
	float absZ = std::abs(penetrationZ);

	//最小のベクトルを求める
	if (absX < absZ)
	{
		penetration.x = penetrationX;
	}
	else
	{
		penetration.z = penetrationZ;
	}

	return penetration;
}

void FreezeEnemy::MoveOnIce()
{
	// フレーム間の時間差（秒）
	float deltaTime = 1.0f / 60.0f;

	// 摩擦による減速を適用
	velocity_ *= frictionOnIce_;

	// 速度が非常に小さくなったら停止する
	if (velocity_.Length() < 0.001f)
	{
		velocity_ = { 0.0f, 0.0f, 0.0f };
	}

	// 位置を更新
	transform_.translate_ += velocity_ * deltaTime;
	position_ = transform_.translate_;
}
