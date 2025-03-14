#include "SpawnPos.h"

void SpawnPos::Initialize()
{
	wtSpawn_.Initialize();
	wtSpawn_.translate_ = position_;
	wtSpawn_.scale_ = { 0.4f,0.4f,0.4f };

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
}

void SpawnPos::Draw(BaseCamera _camera)
{
	spawn_->Draw(wtSpawn_, _camera);
}

void SpawnPos::ImGuiDraw()
{
}
