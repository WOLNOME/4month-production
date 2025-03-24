#pragma once

#include "Input.h"

#include <Framework.h>
#include <vector>
#include <memory>
#include <MyMath.h>
#include <Object3d.h>

class StageSelectObject
{
public:

	void Initialize(const std::string& modelFilePath, const std::string& textureFilePath);
	void Finalize();
	void Update();
	void Draw(BaseCamera _camera);
	void ImGuiDraw();

	void StageSelect();

public: // ゲッター

	bool IsMove() const { return isMove_; }

public: // セッター

	void SetPosition(const Vector3& _pos) { wtField_.translate_ = _pos; }

	void SetStage(uint32_t _stage) { nowStage_ = _stage; }

private:

	// 入力
	Input* input_ = nullptr;

	// モデル
	WorldTransform wtField_{};
	int32_t textureHandleField_ = EOF;
	std::unique_ptr<Object3d> field_ = nullptr;

	float moveLength_ = 0.0f;

	bool isMove_ = false;
	bool isMoveLeft_ = false;
	bool isMoveRight_ = false;

	uint32_t nowStage_ = 0;
};

