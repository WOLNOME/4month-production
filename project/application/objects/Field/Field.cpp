#include "Field.h"
#include "TextureManager.h"

#include "../../engine/appCollider/AppColliderManager.h"
#include "ImGuiManager.h"

void Field::Initialize() {
	wtField_.Initialize();
	wtField_.translate_ = { 0.0f,-1.0f,0.0f };
	wtField_.scale_ = scale_;

	textureHandleField_ = TextureManager::GetInstance()->LoadTexture("grid.png");

	field_ = std::make_unique<Object3d>();
	field_->InitializeModel("cube");

	// 当たり判定関係
	appColliderManager_ = AppColliderManager::GetInstance();

	objectName_ = "Field";
	appCollider_ = std::make_unique<AppCollider>();
	desc =
	{
		//ここに設定
		.owner = this,
		.colliderID = objectName_,
		.shape = AppShape::AppAABB,
		.shapeData = &aabb_,
		.attribute = appColliderManager_->GetNewAttribute(objectName_),
	};
	appCollider_->MakeAABBDesc(desc);
	appColliderManager_->RegisterCollider(appCollider_.get());

	//パーティクル
	particle_ = std::make_unique<Particle>();
	particle_->Initialize("field","StageEffect");
	particle_->emitter_.transform.scale = { 30.0f,1.0f,30.0f };
	particle_->emitter_.isGravity = true;
	particle_->emitter_.gravity = 1.0f;
}

void Field::Finalize() {
	// 各解放処理
	if (appCollider_) {
		appColliderManager_->DeleteCollider(appCollider_.get());
		appCollider_.reset();
	}

	field_.reset();
}

void Field::Update() {
	wtField_.UpdateMatrix();

	wtField_.scale_ = scale_;
	wtField_.rotate_ = rotation_;

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

void Field::SetTextureHandle(std::string _textureHandle)
{
	textureHandleField_ = TextureManager::GetInstance()->LoadTexture(_textureHandle);
}