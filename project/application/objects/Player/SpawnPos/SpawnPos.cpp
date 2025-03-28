#include "SpawnPos.h"

#include "TextureManager.h"
#include "ImGuiManager.h"

void SpawnPos::Initialize()
{
	wtSpawn_.Initialize();
	wtSpawn_.translate_ = position_;
	scale_ = { 0.4f,0.4f,0.4f };
	wtSpawn_.scale_ = scale_;

	textureHandleSpawn_ = TextureManager::GetInstance()->LoadTexture("spawn.png");

	spawn_ = std::make_unique<Object3d>();
	spawn_->InitializeModel("cube");
}

void SpawnPos::Finalize()
{
	spawn_.reset();
}

void SpawnPos::Update()
{
	wtSpawn_.UpdateMatrix();

	wtSpawn_.translate_ = position_;
	wtSpawn_.scale_ = scale_;
	wtSpawn_.rotate_ = rotation_;
}

void SpawnPos::Draw(BaseCamera _camera)
{
	spawn_->Draw(wtSpawn_, _camera,nullptr,textureHandleSpawn_);
}

void SpawnPos::ImGuiDraw()
{
	ImGui::Text("SpawnPos");

	ImGui::DragFloat3("Position", &position_.x, 0.01f);
	ImGui::DragFloat3("Scale", &scale_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &rotation_.x, 0.01f);
}
