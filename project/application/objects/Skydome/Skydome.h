#pragma once
#include "Object3d.h"
#include "WorldTransform.h"
#include "BaseCamera.h"
#include <memory>

class Skydome {
public:
	void Initialize();
	void Update();
	void Draw(const BaseCamera& camera);

private:
	//ワールドトランスフォーム
	WorldTransform wtSkydome_ = {};
	//テクスチャ
	int32_t textureHandle_ = EOF;
	//モデル
	std::unique_ptr<Object3d> skydome_ = nullptr;
};

