#pragma once
#include <memory>
#include <string>
#include <vector>

#include "FanEnemy.h"
#include "FreezeEnemy.h"
#include "IceMist.h"
#include "TackleEnemy.h"
#include "Wind.h"

class GameCamera;
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
	void TackleEnemyTargetUpdate();
	void FreezeEnemyTargetUpdate();
	void UpdateTransform();

	void SetGameCamera(GameCamera* camera);

	void SetSpawnPosition(const Vector3& min, const Vector3& max) { spawnMinPosition_ = min; spawnMaxPosition_ = max; }
	void IsMoveable(bool moveable);
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
	// タックルエネミーのポインタを取得
	TackleEnemy* GetTackleEnemy(uint32_t index) { return tackleEnemies_[index].get(); }
	//扇エネミーのポインタを取得
	FanEnemy* GetFanEnemy(uint32_t index) { return fanEnemies_[index].get(); }
	//風のポインタを取得
	FreezeEnemy* GetFreezeEnemy(uint32_t index) { return freezeEnemies_[index].get(); }
	//タックルエネミーの位置を取得
	Vector3 GetTackleEnemyPosition(uint32_t index) const { return tackleEnemies_[index]->GetPosition(); }
	//扇エネミーの位置を取得
	Vector3 GetFanEnemyPosition(uint32_t index) const { return fanEnemies_[index]->GetPosition(); }
	//凍結エネミーの位置を取得
	Vector3 GetFreezeEnemyPosition(uint32_t index) const { return freezeEnemies_[index]->GetPosition(); }
	void Finalize();
	//全ての敵を取得
	std::vector<GameObject*> GetAllEnemies() {
		std::vector<GameObject*> allEnemies;
		for (auto& enemy : tackleEnemies_) {
			allEnemies.push_back(enemy.get());
		}
		for (auto& enemy : fanEnemies_) {
			allEnemies.push_back(enemy.get());
		}
		for (auto& enemy : freezeEnemies_) {
			allEnemies.push_back(enemy.get());
		}
		return allEnemies;
	}

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
	std::unique_ptr<Audio> fallSE_ = nullptr;
};

