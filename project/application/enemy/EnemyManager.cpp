#include "EnemyManager.h"

#include <random>

#include "imgui.h"
#include "application/objects/Player/Player.h"

void EnemyManager::Initialize(BaseCamera* camera, std::vector<std::unique_ptr<Player>>* players, const std::string& tackleEnemy)
{
	camera_ = camera;
	tackleEnemyPath_ = tackleEnemy;
	players_ = players;
}

void EnemyManager::Update()
{
#ifdef _DEBUG
	ImGui::Begin("EnemyManager");
	ImGui::Text("TackleEnemyCount: %d", tackleEnemies_.size());
	ImGui::DragFloat3("TargetPosition", &targetPosition_.x, 0.1f);
	ImGui::DragFloat3("SpawnMinPosition", &spawnMinPosition_.x, 0.1f);
	ImGui::DragFloat3("SpawnMaxPosition", &spawnMaxPosition_.x, 0.1f);
	ImGui::DragInt("SpawnCount", &spawnCount_, 1.0f, 1, 100);
	if (ImGui::Button("SpawnTackleEnemy"))
	{
		SpawnTackleEnemy(spawnCount_);
	}
	if (ImGui::Button("Tackle"))
	{
		TargetUpdate();
		for (auto& enemy : tackleEnemies_)
		{
			enemy->StartTackle();
		}
	}
	ImGui::End();
#endif
	//ターゲットの更新
	TargetUpdate();
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
	//死んでいるエネミーをリストから削除
	tackleEnemies_.erase(std::remove_if(tackleEnemies_.begin(), tackleEnemies_.end(),
		[](const std::unique_ptr<TackleEnemy>& enemy)
		{
			return !enemy->IsAlive();
		}), 
		tackleEnemies_.end());
}

void EnemyManager::Draw()
{
	//タックルエネミーの描画
	for (auto& enemy : tackleEnemies_)
	{
		enemy->EnemyDraw(*camera_);
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
		Vector3 spawnPosition = { disX(gen), 0.0f, disZ(gen) };
		enemy->SetPosition(spawnPosition);
		enemy->SetTargetPosition(targetPosition_);
		tackleEnemies_.emplace_back(std::move(enemy));
	}
}

void EnemyManager::TargetUpdate()
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

void EnemyManager::Finalize()
{
	for (auto& enemy : tackleEnemies_)
	{
		enemy->Finalize();
	}
}
