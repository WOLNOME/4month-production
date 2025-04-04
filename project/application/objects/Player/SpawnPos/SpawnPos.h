#pragma once

#include <Framework.h>
#include <vector>
#include <memory>
#include <MyMath.h>
#include <Object3d.h>
#include <Particle.h>

#include "../../GameObject/GameObject.h"

class SpawnPos : public GameObject
{
public:

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw(BaseCamera _camera) override;

	// ImGui
	void ImGuiDraw();

	void ParticleStart();

	void ParticleStop();

private:

	// モデル情報
	WorldTransform wtSpawn_{};
	int32_t textureHandleSpawn_ = EOF;
	std::unique_ptr<Object3d> spawn_{};

	// パーティクル
	std::unique_ptr<Particle> particle_{};
};

