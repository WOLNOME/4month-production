#define NOMINMAX // std::min std::max を有効にするためのマクロ定義

#include "Enemy.h"

#include "../../appCollider/AppCollisionManager.h"
#include "ImGuiManager.h"
#include "../Player/Player.h"

#include <limits>

void Enemy::Initialize()
{
	wtEnemy_.Initialize();
	wtEnemy_.translate_ = { 3.0f,8.0f,0.0f };
	wtEnemy_.scale_ = { 0.8f,0.8f,0.8f };
	enemy_ = std::make_unique<Object3d>();
	enemy_->InitializeModel("cube");

	// 当たり判定関係
	appCollisionManager_ = AppCollisionManager::GetInstance();

	objectName_ = "Enemy";
	appCollider_ = std::make_unique<AppCollider>();
	appCollider_->SetOwner(this);
	appCollider_->SetColliderID(objectName_);
	appCollider_->SetShapeData(&aabb_);
	appCollider_->SetShape(AppShape::AppAABB);
	appCollider_->SetAttribute(appCollisionManager_->GetNewAttribute(appCollider_->GetColliderID()));
	//appCollider_->SetOnCollisionTrigger(std::bind(&Enemy::OnCollisionTrigger, this, std::placeholders::_1));
	appCollider_->SetOnCollision(std::bind(&Enemy::OnCollision, this, std::placeholders::_1));
	appCollisionManager_->RegisterCollider(appCollider_.get());

}

void Enemy::Finalize()
{
	enemy_.reset();
}

void Enemy::Update()
{
	wtEnemy_.UpdateMatrix();

	// 当たり判定関係
	aabb_.min = wtEnemy_.translate_ - wtEnemy_.scale_;
	aabb_.max = wtEnemy_.translate_ + wtEnemy_.scale_;
	appCollider_->SetPosition(wtEnemy_.translate_);

	// 移動
	Move();

	// 場外処理
	OutOfField();

	wtEnemy_.translate_ += moveVel_;
}

void Enemy::Draw()
{
	enemy_->Draw(wtEnemy_, *camera_);
}

void Enemy::Move()
{
	// 球面線形補間により、今の速度と自キャラへのベクトルを内挿し、新たな速度とする
	moveVel_ = 1.0f * (MyMath::Slerp(moveVel_, toPlayer_, 0.1f));

	// 進行方向に見た目の回転を合わせる
	// Y軸周り角度(θy)

	rotation_.y = std::atan2(moveVel_.x, moveVel_.z);
	rotation_.x = 0.0f;

	moveVel_ /= 20.0f;

	moveVel_.y = 0.0f;
	wtEnemy_.translate_ += moveVel_;
}

void Enemy::OutOfField()
{
	if (isGround_ == false)
	{
		wtEnemy_.translate_.y -= fallSpeed_;
	}

	isGround_ = false;

	if (wtEnemy_.translate_.y < -30.0f)
	{
		isGround_ = true;
	}
}

void Enemy::ImGuiDraw()
{
	ImGui::Begin("Enemy");

	ImGui::SliderFloat3("EnemyPos", &wtEnemy_.translate_.x, -20.0f, 20.0f);

	ImGui::SliderFloat3("EnemySpeed", &moveVel_.x, -5.0f, 5.0f);

	ImGui::Text("isGround_ : %s", isGround_ ? "true" : "false");

	if (ImGui::Button("ReSetPos"))
	{
		wtEnemy_.translate_ = { 1.0f,3.0f,0.0f };
	}

	ImGui::Text("aabb.max: %.3f %.3f %.3f", aabb_.max.x, aabb_.max.y, aabb_.max.z);
	ImGui::Text("aabb.min: %.3f %.3f %.3f", aabb_.min.x, aabb_.min.y, aabb_.min.z);

	ImGui::End();
}

void Enemy::OnCollision(const AppCollider* _other)
{
	if (_other->GetColliderID() == "Field")
	{
		isGround_ = true;
	}
}

void Enemy::SetPlayerPos(const std::vector<std::unique_ptr<Player>>& player)
{
	playerPos_.clear();
	float minDistance = std::numeric_limits<float>::max();
	Vector3 closestPlayerPos;

	for (const auto& player : player)
	{
		Vector3 playerPos = player->GetPosition();
		playerPos_.push_back(playerPos);

		// プレイヤーとの距離を計算
		Vector3 toPlayer = playerPos - wtEnemy_.translate_;
		float distance = toPlayer.Length();

		// 最も近いプレイヤーを見つける
		if (distance < minDistance)
		{
			minDistance = distance;
			closestPlayerPos = playerPos;
		}
	}

	// 最も近いプレイヤーとのベクトルを設定
	toPlayer_ = closestPlayerPos - wtEnemy_.translate_;
	toPlayer_ = MyMath::Normalize(toPlayer_);
}
