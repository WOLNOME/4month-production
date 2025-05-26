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

	// パーティクルの開始
	void ParticleStart();

	// パーティクルの停止
	void ParticleStop();

	// シェイク
	void Shake();

	void UpdateTransform() { wtSpawn_.UpdateMatrix(); }

public: // セッター

	// シェイク切り替え
	void IsShaking(bool isShaking) { isShaking_ = isShaking; }

private:

	// モデル情報
	WorldTransform wtSpawn_{};
	int32_t textureHandleSpawn_ = EOF;
	std::unique_ptr<Object3d> spawn_{};

	// パーティクル
	std::unique_ptr<Particle> particle_{};

	// 揺れ中かどうかを管理するフラグ
	bool isShaking_ = false; 
	// 揺れの強さ
	const float shakePower_ = 0.1f;   

};

