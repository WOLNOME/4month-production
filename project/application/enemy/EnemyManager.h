#pragma once
#include <memory>
#include <string>
#include <vector>

#include "TackleEnemy.h"


class Player;

class EnemyManager
{
public:
	void Initialize(BaseCamera* camera, std::vector<std::unique_ptr<Player>>* players, const std::string& tackleEnemy);
	void Update();
	void Draw();
	void SpawnTackleEnemy(uint32_t count);
	void TargetUpdate();
	Vector3 GetTargetPosition() const { return targetPosition_; }
	void Finalize();
private:
	BaseCamera* camera_ = nullptr;
	std::vector<std::unique_ptr<TackleEnemy>> tackleEnemies_;
	std::string tackleEnemyPath_;
	Vector3 targetPosition_ = { 0.0f,1.0f,3.0f };
	Vector3 spawnMinPosition_ = { -10.0f,1.0f,-10.0f };
	Vector3 spawnMaxPosition_ = { 10.0f,1.0f,10.0f };
	int spawnCount_ = 1;
	std::vector<std::unique_ptr<Player>>* players_ = nullptr; // プレイヤーリストのポインタ
};

