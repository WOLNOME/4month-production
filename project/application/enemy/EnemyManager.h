#pragma once
#include <memory>
#include <string>
#include <vector>

#include "TackleEnemy.h"

class EnemyManager
{
public:
	void Initialize(BaseCamera* camera, const std::string& tackleEnemy);
	void Update();
	void Draw();
	void SpawnTackleEnemy(uint32_t count);
	Vector3 GetTargetPosition() const { return targetPosition_; }
private:
	BaseCamera* camera_ = nullptr;
	std::vector<std::unique_ptr<TackleEnemy>> tackleEnemies_;
	std::string tackleEnemyPath_;
	Vector3 targetPosition_ = { 0.0f,0.0f,3.0f };
	Vector3 spawnMinPosition_ = { -10.0f,0.0f,-10.0f };
	Vector3 spawnMaxPosition_ = { 10.0f,0.0f,10.0f };
	int spawnCount_ = 1;
};

