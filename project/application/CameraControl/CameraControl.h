#pragma once
#include "Vector3.h"
#include <vector>
#include <memory>
class BaseCamera;
class EnemyManager;
class Player;

class CameraControl
{
public:

	void Initialize(const Vector3& cameraRotate);

	void Update(BaseCamera* camera, std::vector<std::unique_ptr<Player>>& players, EnemyManager* enemyManager);

	void OnGameEnd(uint32_t playerNum, uint32_t EnemyNum);

	//ゲームが終了したかどうかを取得する
	bool IsGameEnd();

private:

	void UpdateZoomIn(BaseCamera* camera);

	void UpdateZoomOut(BaseCamera* camera);

	void CalculateNearestPosition(std::vector<std::unique_ptr<Player>>& players, EnemyManager* enemyManager);

	// カメラを引くフラグ
	bool isZoomOut_ = false;
	// カメラを寄せるフラグ
	bool isZoomIn_ = false;
	// カメラのイージング経過時間
	float cameraEaseTime_ = 0.0f;
	// カメラのイージング時間
	const float cameraEaseDuration_ = 2.0f;
	// カメラの開始位置
	Vector3 cameraStartPosition_ = { 0.0f,70.0f,-50.0f };
	// カメラの終了位置
	Vector3 cameraEndPosition_ = { 0.0f, 50.0f, -30.0f };
	// 最も近いプレイヤーの位置
	Vector3 nearestPlayerPos_ = {};
	//　最も近いプレイヤーの番号
	uint32_t nearestPlayerNum_ = 0;
	// 最も近い敵の位置
	Vector3 nearestEnemyPos_ = {};
	// 最も近い敵の番号
	uint32_t nearestEnemyNum_ = 0;
	// 最も近い敵の種類
	std::string nearestEnemyType_ = "";
	//　待機時間
	float waitTime_ = 0.0f;
	// 待機時間の長さ
	float waitTimeDuration_ = 0.13f;

	Vector3 cameraRotate_;

};

