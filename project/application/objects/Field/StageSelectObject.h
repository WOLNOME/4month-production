#pragma once

#include "Input.h"

#include <Framework.h>
#include <vector>
#include <memory>
#include <MyMath.h>
#include <Object3d.h>
#include <Particle.h>

enum class StageNum {
	Tutorial,
	Stage1,
	Stage2,
	Stage3,
	Stage4,
	Stage5,
	StageNumMax
};

class StageSelectObject
{
public:

	void Initialize(const std::string& modelFilePath, const std::string& textureFilePath, int stageNum);
	void Finalize();
	void Update();
	void Draw(BaseCamera _camera);
	void ImGuiDraw();

	void StageSelect();

	void SetStagePosition(uint32_t setStage);

public: // ゲッター

	bool IsMove() const { return isMove_; }

public: // セッター

	void SetPosition(const Vector3& _pos) { wtField_.translate_ = _pos; }

	void SetStage(uint32_t _stage) { nowStage_ = _stage; }

private:
	//エネミージャンプ
	void EnemyJump();

private:

	// 入力
	Input* input_ = nullptr;

	// モデル
	WorldTransform wtField_{};
	int32_t textureHandleField_ = EOF;
	std::unique_ptr<Object3d> field_ = nullptr;

	std::vector<std::pair<WorldTransform, std::unique_ptr<Object3d>>> enemies_{};	//敵
	std::vector<Vector3> enemyVelocity_{};
	std::vector<std::pair<WorldTransform, std::unique_ptr<Object3d>>> gimmicks_{};	//ギミック

	float moveLength_ = 0.0f;

	bool isMove_ = false;
	bool isMoveLeft_ = false;
	bool isMoveRight_ = false;

	uint32_t nowStage_ = 0;

	// ステージ番号
	StageNum stageNum_;

};

