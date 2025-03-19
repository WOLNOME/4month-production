#pragma once

#include <Framework.h>
#include <vector>
#include <memory>
#include <MyMath.h>
#include <Object3d.h>

#include "../../appCollider/AppCollider.h"
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

private: // 衝突判定

	void OnCollision(const AppCollider* _other);

private:

	// モデル情報
	WorldTransform wtField_{};
	int32_t textureHandleField_ = EOF;
	std::unique_ptr<Object3d> field_ = nullptr;

	// 当たり判定関係
	AppCollisionManager* appCollisionManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_ = nullptr;
	AppAABB aabb_{};

};

