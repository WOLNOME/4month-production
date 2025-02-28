#include "Field.h"

#include "../../appCollider/AppCollisionManager.h"
#include "ImGuiManager.h"

void Field::Initialize()
{
	wtField_.Initialize();
	wtField_.translate_ = { 0.0f,-0.5f,0.0f };
	wtField_.scale_ = { 10.0f,2.0f,10.0f };

	field_ = std::make_unique<Object3d>();
	field_->InitializeModel("cube");

	// 当たり判定関係
	appCollisionManager_ = AppCollisionManager::GetInstance();

	objectName_ = "Field";
	appCollider_ = std::make_unique<AppCollider>();
	appCollider_->SetOwner(this);
	appCollider_->SetColliderID(objectName_);
	appCollider_->SetShapeData(&aabb_);
	appCollider_->SetShape(AppShape::AppAABB);
	appCollider_->SetAttribute(appCollisionManager_->GetNewAttribute(appCollider_->GetColliderID()));
	appCollider_->SetOnCollision(std::bind(&Field::OnCollision, this, std::placeholders::_1));
	appCollisionManager_->RegisterCollider(appCollider_.get());

}

void Field::Finalize()
{
	field_.reset();
}

void Field::Update()
{
	wtField_.UpdateMatrix();

	// 当たり判定関係
	aabb_.min = wtField_.translate_ - wtField_.scale_;
	aabb_.max = wtField_.translate_ + wtField_.scale_;
	appCollider_->SetPosition(wtField_.translate_);
}

void Field::Draw()
{
	field_->Draw(wtField_, *camera_);
}

void Field::ImGuiDraw()
{
	ImGui::Begin("Field");

	ImGui::SliderFloat3("FieldSize", &scale_.x, -20.0f, 20.0f);

	ImGui::End();
}

void Field::OnCollision(const AppCollider* _other)
{
	_other;
	// 何もしない
}
