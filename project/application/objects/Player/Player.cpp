#include "Player.h"

#include "../../appCollider/AppCollisionManager.h"
#include "ImGuiManager.h"

void Player::Initialize()
{
	input_ = Input::GetInstance();

	// プレーヤーモデル
	wtPlayer_.Initialize();
	wtPlayer_.scale_ = { 1.0f,1.0f,1.0f };
	player_ = std::make_unique<Object3d>();
	player_->InitializeModel("player");
	

	// 当たり判定関係
	appCollisionManager_ = AppCollisionManager::GetInstance();
	
	objectName_ = "Player";
	appCollider_ = std::make_unique<AppCollider>();
	appCollider_->SetOwner(this);
	appCollider_->SetColliderID(objectName_);
	appCollider_->SetShapeData(&aabb_);
	appCollider_->SetShape(AppShape::AppAABB);
	appCollider_->SetAttribute(appCollisionManager_->GetNewAttribute(appCollider_->GetColliderID()));
	appCollider_->SetOnCollisionTrigger(std::bind(&Player::OnCollisionTrigger, this, std::placeholders::_1));
	appCollider_->SetOnCollision(std::bind(&Player::OnCollision, this, std::placeholders::_1));
	appCollisionManager_->RegisterCollider(appCollider_.get());

	
}

void Player::Finalize()
{
	// 各解放処理
	if (appCollider_)
	{
		appCollisionManager_->DeleteCollider(appCollider_.get());
		appCollider_.reset();
	}

	player_.reset();
}

void Player::Update()
{
	
	
	// ノックバック中は移動、攻撃できない
	if (knockBackTime_ > 0.0f)
	{
		knockBackTime_ -= 1.0f;
	}
	else if(!isAftertaste_)
	{
		//氷の上にいるとき
		if (onIce_)
		{
			//移動
			MoveOnIce();

			// 攻撃	
			AttackOnIce();
		}
		else
		{
			// 移動
			Move();

			// 攻撃	
			Attack();
		}
	}

	//氷の上にいるとき
	if (onIce_)
	{
		MovePositionOnIce();
	}
	else
	{
		// 位置更新
		MovePosition();
	}
	

	// 場外処理
	OutOfField();

	wtPlayer_.UpdateMatrix();

	// 当たり判定関係
	aabb_.min = wtPlayer_.translate_ - wtPlayer_.scale_;
	aabb_.max = wtPlayer_.translate_ + wtPlayer_.scale_;
	appCollider_->SetPosition(wtPlayer_.translate_);

	onIce_ = false;
}

void Player::Draw(BaseCamera _camera)
{
	if (!isDead_)
	{
		player_->Draw(wtPlayer_, _camera);
	}
}

void Player::Move()
{
	// 摩擦による減速を適用
	moveVel_ *= friction_;

	// 地面にいるとき
	if (isGround_ && !isStop_)
	{
		// 移動
		if (input_->PushKey(DIK_W))
		{
			moveVel_.z += moveSpeed_.z * slowRate_;
		}
		if (input_->PushKey(DIK_S))
		{
			moveVel_.z -= moveSpeed_.z * slowRate_;
		}
		if (input_->PushKey(DIK_A))
		{
			moveVel_.x -= moveSpeed_.x * slowRate_;
		}
		if (input_->PushKey(DIK_D))
		{
			moveVel_.x += moveSpeed_.x * slowRate_;
		}
	}

	// 速度が非常に小さくなったら停止する
	if (moveVel_.Length() < 0.01f)
	{
		moveVel_ = { 0.0f, 0.0f, 0.0f };
	}

	wtPlayer_.translate_ += moveVel_;
	position_ = wtPlayer_.translate_;

}

void Player::MovePosition()
{
	// フレーム間の時間差（秒）
	float deltaTime = 1.0f / 60.0f;

	// 摩擦による減速を適用
	Vector3 friction = -moveVel_ * attackFriction_ * deltaTime;
	moveVel_ += friction;

	// 速度が非常に小さくなったら停止する
	if (moveVel_.Length() < 0.01f)
	{
		return;
	}
	else if (moveVel_.Length() < 1.5f)
	{
		isAftertaste_ = false;
	}

	// 位置を更新
	wtPlayer_.translate_ += moveVel_ * deltaTime;
	position_ = wtPlayer_.translate_;
}

void Player::OutOfField()
{
	if (isGround_ == false)
	{
		wtPlayer_.translate_.y -= fallSpeed_;
	} 
	else
	{
		wtPlayer_.translate_.y = 1.0f;
	}

	if (wtPlayer_.translate_.y < -10.0f)
	{
		isDead_ = true;
		isGround_ = true;
	}

	isGround_ = false;
}

void Player::Attack()
{
	if (input_->TriggerKey(DIK_SPACE) && !isAttack_ && isChargeMax_)
	{
		isAttack_ = true;
		isChargeMax_ = false;

		// 攻撃時間リセット
		attackTimeCounter_ = attackTime_;

	}

	if (isAttack_)
	{
		moveVel_ *= 1.5f * attackFriction_;

		attackTimeCounter_ -= 1.0f;

		Vector3 moveFriction_ = moveVel_ * attackFriction_ * (1.0f/60.0f);
		moveVel_ += moveFriction_;
		wtPlayer_.translate_ += moveVel_ * slowRate_;
		position_ = wtPlayer_.translate_;
	}

	if (attackTimeCounter_ <= 0.0f)
	{
		isAttack_ = false;
		isStop_ = false;
	}
}

void Player::ImGuiDraw()
{
	// プレイヤー
	ImGui::Begin("Player");

	ImGui::SliderFloat3("PlayerPos", &wtPlayer_.translate_.x, -20.0f, 20.0f);

	ImGui::SliderFloat3("PlayerSpeed", &moveVel_.x, -5.0f, 5.0f);

	ImGui::Text("isGround_ : %s", isGround_ ? "true" : "false");

	if (ImGui::Button("ReSetPos"))
	{
		wtPlayer_.translate_ = { 0.0f,3.0f,0.0f };
	}

	ImGui::Text("aabb.max: %.3f %.3f %.3f", aabb_.max.x, aabb_.max.y, aabb_.max.z);
	ImGui::Text("aabb.min: %.3f %.3f %.3f", aabb_.min.x, aabb_.min.y, aabb_.min.z);

	ImGui::End();

}

void Player::OnCollision(const AppCollider* _other)
{
	// フィールドの内にいるかどうか
	if (_other->GetColliderID() == "Field")
	{
		isGround_ = true;
	}
	else if (_other->GetColliderID() == "Obstacle")
	{
		wtPlayer_.translate_ += ComputePenetration(*_other->GetAABB());
		wtPlayer_.UpdateMatrix();

		// 当たり判定関係
		aabb_.min = wtPlayer_.translate_ - wtPlayer_.scale_;
		aabb_.max = wtPlayer_.translate_ + wtPlayer_.scale_;
		appCollider_->SetPosition(wtPlayer_.translate_);
	}
	else if (_other->GetColliderID() == "Bumper")
	{
		Vector3 penetration = ComputePenetration(*_other->GetAABB());
		wtPlayer_.translate_ += penetration;
		penetration.Normalize();
		// ノックバック
		moveVel_ = penetration;
		moveVel_ *= 20.0f;
		moveVel_.y = 0.0f;
		// ノックバックタイマー
		knockBackTime_ = 30.0f;

		isAttack_ = false;
	}
	else if (_other->GetColliderID() == "IceFloor")
	{
		onIce_ = true;
	}

	// どちらも攻撃していなくてノックバック中でないとき
	if (_other->GetColliderID() == "TackleEnemy" && !_other->GetOwner()->IsAttack() && !isAttack_ && !isAftertaste_)
	{
		// プレイヤーの速度を取得
		Vector3 playerVelocity = moveVel_;

		// 減速係数を設定
		float decelerationFactor = 0.5f; // 速度を50%に減少させる

		// プレイヤーの速度を減少させる
		playerVelocity *= decelerationFactor;

		// プレイヤーの速度を更新
		moveVel_ = playerVelocity;

	}

	// プレイヤー同士の衝突
	if (_other->GetColliderID() == "Player")
	{
		// プレイヤー同士の衝突処理
		Vector3 playerPosition = wtPlayer_.translate_;
		Vector3 otherPlayerPosition = _other->GetOwner()->GetPosition();

		// プレイヤー同士が重ならないようにする
		Vector3 direction = playerPosition - otherPlayerPosition;
		direction.Normalize();
		float distance = 2.5f; // プレイヤー同士の間の距離を調整するための値

		// 互いに重ならないように少しずつ位置を調整
		if ((playerPosition - otherPlayerPosition).Length() < distance)
		{
			playerPosition += direction * 0.1f; // 微調整のための値
			playerPosition.y = 1.0f;
			wtPlayer_.translate_ = playerPosition;
			position_ = wtPlayer_.translate_;
		}
	}

	//アイスミストに当たっている間速度低下
	slowRate_ = 1.0f;
	if (_other->GetColliderID() == "IceMist")
	{
		slowRate_ = 0.25f;
	}

}

void Player::OnCollisionTrigger(const AppCollider* _other)
{
	if (_other->GetColliderID() == "TackleEnemy")
	{
		// 攻撃が当たったとき攻撃を止める
		if (isAttack_)
		{
			isStop_ = true;
		}

		// エネミーの攻撃を食らったとき
		if (_other->GetOwner()->IsAttack() && !isAttack_)
		{
			isAftertaste_ = true;

			// 当たったエネミーの位置を取得
			enemyPosition_ = _other->GetOwner()->GetPosition();

			attackToEnemy_ = wtPlayer_.translate_ - enemyPosition_;

			// ノックバック
			moveVel_ = attackToEnemy_;
			moveVel_ *= 10.0f;
			moveVel_.y = 0.0f;
			// ノックバックタイマー
			knockBackTime_ = 40.0f;
		}
	}

	// 風に当たったらノックバック
	if (_other->GetColliderID() == "Wind" && !isAttack_)
	{
		isAftertaste_ = true;
		//当たった風の位置を取得
		Vector3 windDirection = wtPlayer_.translate_ - _other->GetOwner()->GetPosition();
		// ノックバック
		moveVel_ += windDirection * 2.0f;
		//moveVel_ *= 2.0f;
		moveVel_.y = 0.0f;
		// ノックバックタイマー
		knockBackTime_ = 25.0f;
	}
}

Vector3 Player::ComputePenetration(const AppAABB& otherAABB)
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

void Player::MoveOnIce()
{

	// 地面にいるとき
	if (isGround_ && !isStop_)
	{
		// 移動
		if (input_->PushKey(DIK_W))
		{
			moveVel_.z += moveSpeedOnIce_;
		}
		if (input_->PushKey(DIK_S))
		{
			moveVel_.z -= moveSpeedOnIce_;
		}
		if (input_->PushKey(DIK_A))
		{
			moveVel_.x -= moveSpeedOnIce_;
		}
		if (input_->PushKey(DIK_D))
		{
			moveVel_.x += moveSpeedOnIce_;
		}
	}

	

}

void Player::AttackOnIce()
{
	if (input_->TriggerKey(DIK_SPACE) && !isAttack_)
	{
		isAttack_ = true;

		// 攻撃時間リセット
		attackTimeCounter_ = attackTime_;

	}

	if (isAttack_)
	{
		moveVel_ *= 1.5f * attackFriction_;

		attackTimeCounter_ -= 1.0f;

		Vector3 moveFriction_ = moveVel_ * attackFriction_ * (1.0f / 60.0f);
		moveVel_ += moveFriction_;
		wtPlayer_.translate_ += moveVel_;
		position_ = wtPlayer_.translate_;
	}

	if (attackTimeCounter_ <= 0.0f)
	{
		isAttack_ = false;
		isStop_ = false;
	}
}

void Player::MovePositionOnIce()
{

	// 摩擦による減速を適用
	moveVel_ *= frictionOnIce_ * slowRate_;

	//速度を最高速度以下に抑える
	if (moveVel_.Length() > MaxSpeedOnIce_)
	{
		moveVel_.Normalize();
		moveVel_ *= MaxSpeedOnIce_;
	}
	// 速度が非常に小さくなったら停止する
	else if (moveVel_.Length() < 0.001f)
	{
		moveVel_ = { 0.0f, 0.0f, 0.0f };
	}
	if (moveVel_.Length() < 0.04f)
	{
		isAftertaste_ = false;
	}

	// 位置を更新
	wtPlayer_.translate_ += moveVel_;
	position_ = wtPlayer_.translate_;

}
