#pragma once

#include <Framework.h>
#include <vector>
#include <memory>
#include <MyMath.h>
#include <Object3d.h>

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

private:

	// モデル情報
	WorldTransform wtSpawn_{};
	int32_t textureHandleSpawn_ = EOF;
	std::unique_ptr<Object3d> spawn_{};

};

