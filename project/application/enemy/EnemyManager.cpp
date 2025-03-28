#include "EnemyManager.h"

#include <random>

#include "imgui.h"
#include "application/objects/Player/Player.h"

void EnemyManager::Initialize(BaseCamera* camera, std::vector<std::unique_ptr<Player>>* players, const std::string& tackleEnemy, const std::string& fanEnemy, const std::string& freezeEnemy)
{
	camera_ = camera;
	tackleEnemyPath_ = tackleEnemy;
	fanEnemyPath_ = fanEnemy;
	freezeEnemyPath_ = freezeEnemy;
	players_ = players;
}

void EnemyManager::Update()
{
#ifdef _DEBUG
	ImGui::Begin("EnemyManager");
	ImGui::SeparatorText("Count");
	ImGui::Text("Total: %d", GetEnemyCount());
	ImGui::Text("TackleEnemy: %d", tackleEnemies_.size());
	ImGui::Text("FanEnemy: %d", fanEnemies_.size());
	ImGui::Text("FreezeEnemy: %d", freezeEnemies_.size());
	ImGui::Text("Wind: %d", winds_.size());
	ImGui::Text("IceMist: %d", iceMists_.size());
	ImGui::SeparatorText("Spawn");
	ImGui::DragFloat3("SpawnMinPosition", &spawnMinPosition_.x, 0.1f);
	ImGui::DragFloat3("SpawnMaxPosition", &spawnMaxPosition_.x, 0.1f);
	ImGui::DragInt("SpawnCount", &spawnCount_, 1.0f, 1, 100);
	if (ImGui::Button("SpawnTackleEnemy"))
	{
		SpawnTackleEnemy(spawnCount_);
	}
	if (ImGui::Button("SpawnFanEnemy"))
	{
		SpawnFanEnemy(spawnCount_);
	}
	if (ImGui::Button("SpawnFreezeEnemy"))
	{
		SpawnFreezeEnemy(spawnCount_);
	}
	if (ImGui::Button("Tackle"))
	{
		for (auto& enemy : tackleEnemies_)
		{
			enemy->StartTackle();
		}
	}
	ImGui::End();
#endif
	//ターゲットの更新
	TackleEnemyTargetUpdate();
	FreezeEnemyTargetUpdate();
	//タックルエネミーの更新
	for (auto& enemy : tackleEnemies_)
	{
		enemy->EnemyUpdate();
		//死んでいたら
		if (!enemy->IsAlive())
		{
			enemy->Finalize();
		}
	}
	//ファンエネミーの更新
	for (auto& enemy : fanEnemies_)
	{
		enemy->EnemyUpdate();
	}
	//風の更新
	for (auto& wind : winds_)
	{
		wind->Update();
	}
	//フリーズエネミーの更新
	for (auto& enemy : freezeEnemies_)
	{
		enemy->EnemyUpdate();
	}
	//アイスミストの更新
	for (auto& iceMist : iceMists_)
	{
		iceMist->Update();
	}

	//死んでいるエネミーをリストから削除
	tackleEnemies_.erase(std::remove_if(tackleEnemies_.begin(), tackleEnemies_.end(),
		[](const std::unique_ptr<TackleEnemy>& enemy)
		{
			return !enemy->IsAlive();
		}), 
		tackleEnemies_.end());
	//死んでいるエネミーをリストから削除
	fanEnemies_.erase(std::remove_if(fanEnemies_.begin(), fanEnemies_.end(),
		[](const std::unique_ptr<FanEnemy>& enemy)
		{
			return !enemy->IsAlive();
		}),
		fanEnemies_.end());
	//死んでいる風をリストから削除
	winds_.erase(std::remove_if(winds_.begin(), winds_.end(),
		[](const std::unique_ptr<Wind>& wind)
		{
			return !wind->IsAlive();
		}),
		winds_.end());
	//死んでいるエネミーをリストから削除
	freezeEnemies_.erase(std::remove_if(freezeEnemies_.begin(), freezeEnemies_.end(),
		[](const std::unique_ptr<FreezeEnemy>& enemy)
		{
			return !enemy->IsAlive();
		}),
		freezeEnemies_.end());
	//死んでいるアイスミストをリストから削除
	iceMists_.erase(std::remove_if(iceMists_.begin(), iceMists_.end(),
		[](const std::unique_ptr<IceMist>& iceMist)
		{
			return !iceMist->IsAlive();
		}),
		iceMists_.end());
}

void EnemyManager::Draw()
{
	//タックルエネミーの描画
	for (auto& enemy : tackleEnemies_)
	{
		enemy->EnemyDraw(*camera_);
	}
	//ファンエネミーの描画
	for (auto& enemy : fanEnemies_)
	{
		enemy->EnemyDraw(*camera_);
	}
	//風の描画
	for (auto& wind : winds_)
	{
		wind->Draw(*camera_);
	}
	//フリーズエネミーの描画
	for (auto& enemy : freezeEnemies_)
	{
		enemy->EnemyDraw(*camera_);
	}
	//アイスミストの描画
	for (auto& iceMist : iceMists_)
	{
		iceMist->Draw(*camera_);
	}
}

void EnemyManager::SpawnTackleEnemy(uint32_t count)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> disX(spawnMinPosition_.x, spawnMaxPosition_.x);
	std::uniform_real_distribution<float> disZ(spawnMinPosition_.z, spawnMaxPosition_.z);

	for (uint32_t i = 0; i < count; i++)
	{
		auto enemy = std::make_unique<TackleEnemy>();
		enemy->EnemyInitialize(tackleEnemyPath_);
		Vector3 spawnPosition = { disX(gen), 1.5f, disZ(gen) };
		enemy->SetPosition(spawnPosition);
		enemy->SetTargetPosition(targetPosition_);
		tackleEnemies_.emplace_back(std::move(enemy));
	}
}

void EnemyManager::SpawnFanEnemy(uint32_t count)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> disX(spawnMinPosition_.x, spawnMaxPosition_.x);
	std::uniform_real_distribution<float> disZ(spawnMinPosition_.z, spawnMaxPosition_.z);
	for (uint32_t i = 0; i < count; i++)
	{
		auto enemy = std::make_unique<FanEnemy>();
		enemy->SetEnemyManager(this);
		enemy->EnemyInitialize(fanEnemyPath_);
		Vector3 spawnPosition = { disX(gen), 1.5f, disZ(gen) };
		enemy->SetPosition(spawnPosition);
		fanEnemies_.emplace_back(std::move(enemy));
	}
}

void EnemyManager::SpawnWind(const Vector3& position, const Vector3& direction)
{
	auto wind = std::make_unique<Wind>();
	wind->Initialize("Cube", position, direction);
	winds_.emplace_back(std::move(wind));					
}

void EnemyManager::SpawnFreezeEnemy(uint32_t count)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> disX(spawnMinPosition_.x, spawnMaxPosition_.x);
	std::uniform_real_distribution<float> disZ(spawnMinPosition_.z, spawnMaxPosition_.z);
	for (uint32_t i = 0; i < count; i++)
	{
		auto enemy = std::make_unique<FreezeEnemy>(this);
		enemy->EnemyInitialize(freezeEnemyPath_);
		Vector3 spawnPosition = { disX(gen), 1.5f, disZ(gen) };
		enemy->SetPosition(spawnPosition);
		freezeEnemies_.emplace_back(std::move(enemy));
	}
}

void EnemyManager::SpawnIceMist(const Vector3& position, const Vector3& velocity)
{
	auto iceMist = std::make_unique<IceMist>();
	iceMist->Initialize("Cube", position, velocity);
	iceMists_.emplace_back(std::move(iceMist));
}

void EnemyManager::TackleEnemyTargetUpdate()
{
	//タックルエネミーの更新
	for (auto& enemy : tackleEnemies_)
	{
		//敵から一番近いプレイヤーを探す
		Vector3 target{};
		for (auto& player : *players_)
		{
			if (target.Length() == 0.0f)
			{
				target = player->GetPosition();
			}
			else
			{
				if ((enemy->GetPosition() - player->GetPosition()).Length() < (enemy->GetPosition() - target).Length())
				{
					target = player->GetPosition();
				}
			}
		}
		enemy->SetTargetPosition(target);
	}
	
}

void EnemyManager::FreezeEnemyTargetUpdate()
{
	//フリーズエネミーの更新
	for (auto& enemy : freezeEnemies_)
	{
		//敵から一番近いプレイヤーを探す
		Vector3 target{};
		for (auto& player : *players_)
		{
			if (target.Length() == 0.0f)
			{
				target = player->GetPosition();
			}
			else
			{
				if ((enemy->GetPosition() - player->GetPosition()).Length() < (enemy->GetPosition() - target).Length())
				{
					target = player->GetPosition();
				}
			}
		}
		enemy->SetTargetPosition(target);
	}
}

void EnemyManager::Finalize()
{
	for (auto& enemy : tackleEnemies_)
	{
		enemy->Finalize();
	}
}
