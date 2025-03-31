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
	// 回転速度の変更
	ChageRotationSpeed();
	// 回転
	transform_.rotate_.y = fmod(transform_.rotate_.y + rotateSpeed_, 2.0f * 3.14159265359f);
	// 風の更新
	FanUpdate();
	// 場外処理
	OutOfField();
	//行列の更新
	transform_.UpdateMatrix();
	// 当たり判定関係
	UpdateCollider();

	onIce_ = false;
}

void FanEnemy::EnemyDraw(const BaseCamera& camera)
{
	object3d_->Draw(transform_, camera);
}

void FanEnemy::OnCollision(const AppCollider* _other)
{
	if (_other->GetColliderID() == "Field")
	{
		// 地面にいる
		isGround_ = true;
	}
	//敵同士の当たり判定
	if (_other->GetColliderID() == "FreezeEnemy" || _other->GetColliderID() == "TackleEnemy" || _other->GetColliderID() == "FanEnemy")
	{
		// 敵の位置
		Vector3 enemyPosition = _other->GetOwner()->GetPosition();

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
	if (_other->GetColliderID() == "Obstacle")
		{
			transform_.translate_ += ComputePenetration(*_other->GetAABB());
			//行列の更新
			transform_.UpdateMatrix();

			// 当たり判定関係
			aabb_.min = transform_.translate_ - transform_.scale_;
			aabb_.max = transform_.translate_ + transform_.scale_;
			appCollider_->SetPosition(transform_.translate_);
		}
		else if (_other->GetColliderID() == "Bumper")
		{
			Vector3 penetration = ComputePenetration(*_other->GetAABB());
			transform_.translate_ += penetration;
			penetration.Normalize();
			// ノックバック
			velocity_ = penetration;
			velocity_ *= 20.0f;
			velocity_.y = 0.0f;

		}
		else if (_other->GetColliderID() == "IceFloor")
		{
			onIce_ = true;
		}
}

void FanEnemy::OnCollisionTrigger(const AppCollider* other)
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
	 // 風の進行方向を計算
	Vector3 direction = { -sin(transform_.rotate_.y), 0.0f, -cos(transform_.rotate_.y) };
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

void FanEnemy::OutOfField()
{
	// 落下速度
	float fallSpeed_ = 0.3f;

	if (isGround_ == false)
	{
		transform_.translate_.y -= fallSpeed_;
	}

	if (transform_.translate_.y < -10.0f)
	{
		isAlive_ = false;
		isGround_ = true;
	}

	isGround_ = false;
}

void FanEnemy::UpdateCollider()
{
	// 当たり判定
	aabb_.min = transform_.translate_ - transform_.scale_;
	aabb_.max = transform_.translate_ + transform_.scale_;
	appCollider_->SetPosition(transform_.translate_);
}

Vector3 FanEnemy::ComputePenetration(const AppAABB& otherAABB)
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

void FanEnemy::MoveOnIce()
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
