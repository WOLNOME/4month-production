#pragma once

#include <Framework.h>
#include <vector>
#include <memory>
#include <MyMath.h>
#include <Object3d.h>
#include <Particle.h>

#include "../../../engine/appCollider/AppCollider.h"
#include "../GameObject/GameObject.h"


class Field : public GameObject
{
public:

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw(BaseCamera _camera) override;

	// ImGui
	void ImGuiDraw();

	void UpdateTransform() { wtField_.UpdateMatrix(); }

public: // セッター

	void SetTextureHandle(std::string _textureHandle);

private:

	// モデル情報
	WorldTransform wtField_{};
	int32_t textureHandleField_ = EOF;
	std::unique_ptr<Object3d> field_ = nullptr;

	// 当たり判定関係
	AppColliderManager* appColliderManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_ = nullptr;
	AppAABB aabb_{};
	AppCollider::AppColliderDesc desc = {};

	// パーティクル
	std::unique_ptr<Particle> particle_ = nullptr;
};

