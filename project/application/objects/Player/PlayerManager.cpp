#include "PlayerManager.h"

#include "ImGuiManager.h"

#include "../../UI/Charge.h"

void PlayerManager::Initialize()
{
	for (uint32_t i = 0; i < 1; ++i)
	{
		auto player = std::make_unique<Player>();

		player->SetPlayerPos(playerSpawnPositions_[0]);
		player->SetScale({ 1.0f, 1.0f, 1.0f });
		player->Initialize();
		player->SetIsChargeMax(charge_->IsChargeMaxPtr());

		players_.push_back(std::move(player));

		if (onPlayerAdded_)
		{
			onPlayerAdded_();
		}
	}

}

void PlayerManager::Finalize()
{
	for (auto& player : players_)
	{
		player->Finalize();
	}
}

void PlayerManager::Update()
{
	// 死んだプレイヤーを削除
	players_.erase(std::remove_if(players_.begin(), players_.end(),
		[this](const std::unique_ptr<Player>& player)
		{
			if (player->IsDead())
			{
				player->Finalize();
				if (onPlayerRemoved_) 
				{
					onPlayerRemoved_();
				}
				return true;
			}
			return false;
		}), players_.end());
	// 通常プレイヤーの更新
	for (auto& player : players_)
	{
		player->Update();
	}
}

void PlayerManager::Draw(BaseCamera _camera)
{
	for (auto& player : players_)
	{
		player->Draw(_camera);
	}
}

void PlayerManager::ImGuiDraw()
{
	for (auto& player : players_)
	{
		player->ImGuiDraw();
	}	
}

void PlayerManager::AddPlayer(std::unique_ptr<Player> newPlayer)
{
	players_.push_back(std::move(newPlayer));
}

void PlayerManager::UpdateTransform()
{
	for (auto& player : players_)
	{
		player->UpdateModel();
	}
}

void PlayerManager::IsMoveable(bool isMoveable)
{
	for (auto& player : players_)
	{
		player->IsMoveable(isMoveable);
	}
}

void PlayerManager::SetSpawnPositions(const std::vector<Vector3>& positions)
{
	playerSpawnPositions_ = positions;
}

void PlayerManager::SetOnPlayerAdded(const std::function<void()>& callback)
{
	onPlayerAdded_ = callback;
}

void PlayerManager::SetOnPlayerRemoved(const std::function<void()>& callback)
{
	onPlayerRemoved_ = callback;
}

Vector3 PlayerManager::GetPlayerPosition(uint32_t index) const
{
	if (index >= players_.size() || !players_[index]) {
		return Vector3(); // または適切な無効値
	}
	return players_[index]->GetPosition();
}

bool PlayerManager::IsPlayerGrounded(uint32_t index) const
{
	if (index >= players_.size() || !players_[index]) {
		return false;
	}
	return players_[index]->IsGround();
}
