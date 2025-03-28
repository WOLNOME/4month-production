#include "Wind.h"
#include "TextureManager.h"

Wind::~Wind()
{
	// 当たり判定解放
	appCollisionManager_->DeleteCollider(appCollider_.get());
	appCollider_.reset();
}

void Wind::Initialize(const std::string& filePath, const Vector3& position, const Vector3& direction)
{
	object3d_ = std::make_unique<Object3d>();
	object3d_->InitializeModel(filePath);
	transform_.Initialize();
	transform_.translate_ = position;
	transform_.scale_ = { 0.5f, 0.5f, 0.5f };
	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };
	textureHandle_ = TextureManager::GetInstance()->LoadTexture("wind.png");
	direction_ = direction;
	startPosition_ = position;
	//当たり判定
	appCollisionManager_ = AppCollisionManager::GetInstance();
	objectName_ = "Wind";
	appCollider_ = std::make_unique<AppCollider>();
	appCollider_->SetOwner(this);
	appCollider_->SetColliderID(objectName_);
	appCollider_->SetShapeData(&aabb_);
	appCollider_->SetShape(AppShape::AppAABB);
	appCollider_->SetAttribute(appCollisionManager_->GetNewAttribute(appCollider_->GetColliderID()));
	appCollider_->SetOnCollisionTrigger(std::bind(&Wind::OnCollisionTrigger, this, std::placeholders::_1));
	appCollider_->SetOnCollision(std::bind(&Wind::OnCollision, this, std::placeholders::_1));
	appCollisionManager_->RegisterCollider(appCollider_.get());
}

void Wind::Update()
{
	// 移動
	const float deltaTime = 1.0f / 60.0f;
	transform_.translate_ += direction_ * speed_ * deltaTime;

	// スタート地点からの距離が一定以上になったら消す
	if ((transform_.translate_ - startPosition_).Length() > range_)
	{
		isAlive_ = false;
	}

	// 行列の更新
	transform_.UpdateMatrix();

	// 当たり判定
	aabb_.min = transform_.translate_ - transform_.scale_;
	aabb_.max = transform_.translate_ + transform_.scale_;
	appCollider_->SetPosition(transform_.translate_);
}

void Wind::Draw(BaseCamera camera)
{
	if (!isAlive_) return;
	object3d_->Draw(transform_, camera, nullptr,textureHandle_);
}

void Wind::OnCollision(const AppCollider* other)
{

}

void Wind::OnCollisionTrigger(const AppCollider* other)
{
	if (other->GetColliderID() == "Obstacle" || other->GetColliderID() == "Bumper")
	{
		isAlive_ = false;
	}
}
