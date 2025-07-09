#pragma once
#include <memory>
#include <string>
#include <vector>

#include "FanEnemy.h"
#include "FreezeEnemy.h"
#include "IceMist.h"
#include "TackleEnemy.h"
#include "Wind.h"

class Player;

class EnemyManager
{
public:
	void Initialize(BaseCamera* camera, std::vector<std::unique_ptr<Player>>* players, const std::string& tackleEnemy, const std::string& fanEnemy, const std::string& freezeEnemy);
	void Update();
	void Draw();
	void SpawnTackleEnemy(uint32_t count);
	void SpawnFanEnemy(uint32_t count);
	void SpawnWind(const Vector3& position, const Vector3& direction, const Vector3& rotate);
	void SpawnFreezeEnemy(uint32_t count);
	void SpawnIceMist(const Vector3& position, const Vector3& velocity);
	void TargetUpdate();
	void UpdateTransform();

	void SetSpawnPosition(const Vector3& min, const Vector3& max) { spawnMinPosition_ = min; spawnMaxPosition_ = max; }
	void IsMoveable(bool moveable);
	Vector3 GetTargetPosition() const { return targetPosition_; }
	// アイスミストの数を取得
	uint32_t GetIceMistCount() const { return static_cast<uint32_t>(iceMists_.size()); }
	// 敵の総数を取得
	uint32_t GetEnemyCount() const { return static_cast<uint32_t>(enemies_.size()); }
	void Finalize();
	//全ての敵を取得
	std::vector<std::unique_ptr<BaseEnemy>>& GetAllEnemies() { return enemies_; }

private:
	BaseCamera* camera_ = nullptr;
	std::vector<std::unique_ptr<BaseEnemy>> enemies_;
	std::vector<std::unique_ptr<Wind>> winds_;
	std::vector<std::unique_ptr<IceMist>> iceMists_;
	std::string tackleEnemyPath_;
	std::string fanEnemyPath_;
	std::string freezeEnemyPath_;
	Vector3 targetPosition_ = { 0.0f,1.0f,3.0f };
	//スポーンの範囲
	Vector3 spawnMinPosition_ = { -20.0f,1.0f,-20.0f };
	Vector3 spawnMaxPosition_ = { 20.0f,1.0f,20.0f };
	int spawnCount_ = 1;
	std::vector<std::unique_ptr<Player>>* players_ = nullptr; // プレイヤーリストのポインタ
	std::unique_ptr<Audio> fallSE_ = nullptr;
};

