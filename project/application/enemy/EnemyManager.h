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
	void SpawnWind(const Vector3& position, const Vector3& direction);
	void SpawnFreezeEnemy(uint32_t count);
	void SpawnIceMist(const Vector3& position, const Vector3& velocity);
	void TackleEnemyTargetUpdate();
	void FreezeEnemyTargetUpdate();

	void SetSpawnPosition(const Vector3& min, const Vector3& max) { spawnMinPosition_ = min; spawnMaxPosition_ = max; }
	Vector3 GetTargetPosition() const { return targetPosition_; }
	// タックルエネミーの数を取得
	uint32_t GetTackleEnemyCount() const { return static_cast<uint32_t>(tackleEnemies_.size()); }
	// 扇エネミーの数を取得
	uint32_t GetFanEnemyCount() const { return static_cast<uint32_t>(fanEnemies_.size()); }
	// 風の数を取得
	uint32_t GetWindCount() const { return static_cast<uint32_t>(winds_.size()); }
	// 凍結エネミーの数を取得
	uint32_t GetFreezeEnemyCount() const { return static_cast<uint32_t>(freezeEnemies_.size()); }
	// アイスミストの数を取得
	uint32_t GetIceMistCount() const { return static_cast<uint32_t>(iceMists_.size()); }
	// 敵の総数を取得
	uint32_t GetEnemyCount() const { return GetTackleEnemyCount() + GetFanEnemyCount() + GetFreezeEnemyCount(); }
	void Finalize();

private:
	BaseCamera* camera_ = nullptr;
	std::vector<std::unique_ptr<TackleEnemy>> tackleEnemies_;
	std::vector<std::unique_ptr<FanEnemy>> fanEnemies_;
	std::vector<std::unique_ptr<Wind>> winds_;
	std::vector<std::unique_ptr<FreezeEnemy>> freezeEnemies_;
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
};

