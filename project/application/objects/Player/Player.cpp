#include "Player.h"

#include "../../appCollider/AppCollisionManager.h"
#include "ImGuiManager.h"

void Player::Initialize()
{
	input_ = Input::GetInstance();

	wtPlayer_.Initialize();
	wtPlayer_.translate_ = { 0.0f,3.0f,0.0f };
	wtPlayer_.scale_ = { 1.0f,1.0f,1.0f };
	player_ = std::make_unique<Object3d>();
	player_->InitializeModel("cube");

	// 当たり判定関係
	appCollisionManager_ = AppCollisionManager::GetInstance();
	
	objectName_ = "Player";
	appCollider_ = std::make_unique<AppCollider>();
	appCollider_->SetOwner(this);
	appCollider_->SetColliderID(objectName_);
	appCollider_->SetShapeData(&aabb_);
	appCollider_->SetShape(AppShape::AppAABB);
	appCollider_->SetAttribute(appCollisionManager_->GetNewAttribute(appCollider_->GetColliderID()));
	//appCollider_->SetOnCollisionTrigger(std::bind(&Player::OnCollisionTrigger, this, std::placeholders::_1));
	appCollider_->SetOnCollision(std::bind(&Player::OnCollision, this, std::placeholders::_1));
	appCollisionManager_->RegisterCollider(appCollider_.get());

}

void Player::Finalize()
{
	player_.reset();
}

void Player::Update()
{
	wtPlayer_.UpdateMatrix();

	// 当たり判定関係
	aabb_.min = wtPlayer_.translate_ - wtPlayer_.scale_;
	aabb_.max = wtPlayer_.translate_ + wtPlayer_.scale_;
	appCollider_->SetPosition(wtPlayer_.translate_);
	
	// 移動
	Move();
	
	// 場外処理
	OutOfField();
	
	// 攻撃	
	Attack();

	wtPlayer_.translate_ += moveVel_;
	position_ = wtPlayer_.translate_;
}

void Player::Draw()
{
	player_->Draw(wtPlayer_, *camera_);
}

void Player::Move()
{
	moveVel_ = { 0.0f,0.0f,0.0f };

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

void Player::OutOfField()
{
	if (isGround_ == false)
	{
		wtPlayer_.translate_.y -= fallSpeed_;
	}

	isGround_ = false;
}

void Player::Attack()
{
	if (input_->TriggerKey(DIK_SPACE))
	{
		isAttack_ = true;
	}

	if (isAttack_)
	{
		moveVel_ *= 3.5f;

		attackTimeCounter_ -= 1.0f;
	}

	if (attackTimeCounter_ <= 0.0f)
	{
		isAttack_ = false;
		attackTimeCounter_ = attackTime_;
	}
}

void Player::ImGuiDraw()
{
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
	if (_other->GetColliderID() == "Field")
	{
		isGround_ = true;
	}
}
