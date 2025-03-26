#include "Field.h"
#include "TextureManager.h"

#include "../../appCollider/AppCollisionManager.h"
#include "ImGuiManager.h"

void Field::Initialize() {
	wtField_.Initialize();
	wtField_.translate_ = { 0.0f,-1.0f,0.0f };
	wtField_.scale_ = scale_;

	textureHandleField_ = TextureManager::GetInstance()->LoadTexture("grid.png");

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

void Field::Finalize() {
	// 各解放処理
	if (appCollider_) {
		appCollisionManager_->DeleteCollider(appCollider_.get());
		appCollider_.reset();
	}

	field_.reset();
}

void Field::Update() {
	wtField_.UpdateMatrix();

	wtField_.scale_ = scale_;

	// 当たり判定関係
	aabb_.min = wtField_.translate_ - wtField_.scale_;
	aabb_.max = wtField_.translate_ + wtField_.scale_;

	appCollider_->SetPosition(wtField_.translate_);
}

void Field::Draw(BaseCamera _camera) {
	field_->Draw(wtField_, _camera, nullptr, textureHandleField_);
}

void Field::ImGuiDraw() {
	ImGui::Begin("Field");

	ImGui::SliderFloat3("FieldSize", &wtField_.scale_.x, 0.0f, 50.0f);
	ImGui::SliderFloat3("FieldPos", &wtField_.translate_.x, -50.0f, 50.0f);

	ImGui::Text("aabb.max: %.3f %.3f %.3f", aabb_.max.x, aabb_.max.y, aabb_.max.z);
	ImGui::Text("aabb.min: %.3f %.3f %.3f", aabb_.min.x, aabb_.min.y, aabb_.min.z);


	ImGui::End();
}

void Field::OnCollision(const AppCollider* _other) {
	_other;
	// 何もしない
}
