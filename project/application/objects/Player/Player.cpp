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
	wtPlayer_.UpdateMatrix();

	// 当たり判定関係
	aabb_.min = wtPlayer_.translate_ - wtPlayer_.scale_;
	aabb_.max = wtPlayer_.translate_ + wtPlayer_.scale_;
	appCollider_->SetPosition(wtPlayer_.translate_);
	
	// ノックバック中は移動、攻撃できない
	if (knockBackTime_ > 0.0f)
	{
		knockBackTime_ -= 1.0f;
	}
	else
	{
		// 移動
		Move();

		// 攻撃	
		Attack();
	}

	// 位置更新
	MovePosition();

	// 場外処理
	OutOfField();

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
			moveVel_.z += moveSpeed_.z;
		}
		if (input_->PushKey(DIK_S))
		{
			moveVel_.z -= moveSpeed_.z;
		}
		if (input_->PushKey(DIK_A))
		{
			moveVel_.x -= moveSpeed_.x;
		}
		if (input_->PushKey(DIK_D))
		{
			moveVel_.x += moveSpeed_.x;
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

		Vector3 moveFriction_ = moveVel_ * attackFriction_ * (1.0f/60.0f);
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

	// どちらも攻撃していないとき
	if (_other->GetColliderID() == "TackleEnemy" && !_other->GetOwner()->IsAttack() && !isAttack_)
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
			// 当たったエネミーの位置を取得
			enemyPosition_ = _other->GetOwner()->GetPosition();

			attackToEnemy_ = wtPlayer_.translate_ - enemyPosition_;

			// ノックバック
			moveVel_ = attackToEnemy_;
			moveVel_ *= 17.0f;
			moveVel_.y = 0.0f;
			// ノックバックタイマー
			knockBackTime_ = 40.0f;
		}
	}
}
