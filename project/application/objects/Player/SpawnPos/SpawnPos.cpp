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

	Shake();

	if (!isShaking_)
	{
		wtSpawn_.translate_ = position_;
	}
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

void SpawnPos::ParticleStart()
{
	// パーティクル
	particle_ = std::make_unique<Particle>();
	particle_->Initialize("Spown", "Basic");

	particle_->emitter_.transform.translate = position_;
}

void SpawnPos::ParticleStop()
{
	if (particle_)
	{
		particle_.reset();
	}
}
void SpawnPos::Shake()
{
	if (!isShaking_) {
		return; // 揺れ中でない場合は何もしない
	}

	// ランダムな揺れを生成
	float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * shakePower_;
	float offsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * shakePower_;
	float offsetZ = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * shakePower_;

	// 揺れをスポーン位置に適用
	wtSpawn_.translate_.x += offsetX;
	wtSpawn_.translate_.y += offsetY;
	wtSpawn_.translate_.z += offsetZ;
}