#include "PlayerSpawnManager.h"

#include "ImGuiManager.h"

#include "../../../UI/Charge.h"

void PlayerSpawnManager::Initialize()
{
	// プレイヤースポーン位置の初期化
	for (uint32_t i = 0; i < playerSpawnNum_; ++i)
	{
		auto playerSpawn = std::make_unique<SpawnPos>();
		playerSpawn->SetPosition(playerSpawnPositions_[i]);
		playerSpawn->Initialize();

		playerSpawn_.push_back(std::move(playerSpawn));
	}

}

void PlayerSpawnManager::Finalize()
{
	for (auto& playerSpawn : playerSpawn_)
	{
		playerSpawn->Finalize();
	}
	playerSpawn_.clear();
	playerSpawnPositions_.clear();

	if (preSpawnedPlayer_) {
		preSpawnedPlayer_->Finalize();
	}
	preSpawnedPlayer_ = nullptr;

}

void PlayerSpawnManager::Update()
{
	for (auto& playerSpawn : playerSpawn_)
	{
		playerSpawn->Update();
	}

	// 準備中プレイヤーの更新
	if (preSpawnedPlayer_) 
	{
		preSpawnedPlayer_->Update();
	}

}

void PlayerSpawnManager::Draw(BaseCamera _camera)
{
	for (auto& playerSpawn : playerSpawn_)
	{
		playerSpawn->Draw(_camera);
	}

	// 準備中プレイヤーの描画
	if (preSpawnedPlayer_) {
		preSpawnedPlayer_->Draw(_camera);
	}

}

void PlayerSpawnManager::ImGuiDraw()
{
	for (auto& playerSpawn : playerSpawn_)
	{
		playerSpawn->ImGuiDraw();
	}

	ImGui::Text("howManyBoogie : %d", &howManyBoogie_);
}

void PlayerSpawnManager::PlayerSpawnRotation()
{
	rotationTimer_ -= 1.0f;

	if (rotationTimer_ == 120.0f && howManyBoogie_ < maxSpawnNum_)
	{
		playerSpawn_[playerSpawnIndex_]->ParticleStart();
		playerSpawn_[playerSpawnIndex_]->IsShaking(true);

		auto prePlayer = std::make_unique<Player>();
		prePlayer->SetPosition(playerSpawnPositions_[playerSpawnIndex_]);
		prePlayer->SetScale({ 0.1f, 0.1f, 0.1f });
		prePlayer->Initialize();
		prePlayer->SetIsChargeMax(charge_->IsChargeMaxPtr());
		prePlayer->IsMoveable(false);

		preSpawnedPlayer_ = std::move(prePlayer);
		preSpawnedPlayer_->Update();
	}

	// 拡大アニメーション処理
	if (preSpawnedPlayer_)
	{
		float t = (120.0f - rotationTimer_) / 120.0f;
		t = std::clamp(t, 0.0f, 1.0f);

		float lerpedScale = std::lerp(0.1f, 1.0f, t);
		Vector3 scale = { lerpedScale, lerpedScale, lerpedScale };
		preSpawnedPlayer_->SetScale(scale);
	}

	// 出現完了後の処理（TryExportSpawnedPlayerで渡すため準備のみ）
	if (rotationTimer_ <= 0.0f && preSpawnedPlayer_)
	{
		playerSpawn_[playerSpawnIndex_]->ParticleStop();
		playerSpawn_[playerSpawnIndex_]->IsShaking(false);

		preSpawnedPlayer_->SetScale({ 1.0f, 1.0f, 1.0f });
		preSpawnedPlayer_->IsMoveable(true);

		rotationTimer_ = rotation_;

		howManyBoogie_++;
		playerSpawnIndex_ = (playerSpawnIndex_ + 1) % playerSpawnNum_;
	}

	//rotationTimer_ -= 1.0f;

	//// スポーン準備：タイマーが120のとき
	//if (rotationTimer_ == 120.0f && howManyBoogie_ < maxSpawnNum_)
	//{
	//	playerSpawn_[playerSpawnIndex_]->ParticleStart();
	//	playerSpawn_[playerSpawnIndex_]->IsShaking(true);

	//	// 小さい状態で非アクティブなプレイヤー生成
	//	auto prePlayer = std::make_unique<Player>();
	//	prePlayer->SetPosition(playerSpawnPositions_[playerSpawnIndex_]);
	//	prePlayer->SetScale({ 0.1f, 0.1f, 0.1f });
	//	prePlayer->Initialize();
	//	prePlayer->SetIsChargeMax(charge_->IsChargeMaxPtr());

	//	prePlayer->IsMoveable(false);

	//	preSpawnedPlayer_ = std::move(prePlayer);

	//	preSpawnedPlayer_->Update();
	//}

	//// 拡大処理（Lerp）
	//if (preSpawnedPlayer_)
	//{
	//	float t = (120.0f - rotationTimer_) / 120.0f;
	//	t = std::clamp(t, 0.0f, 1.0f);

	//	// Lerpでスケール補間
	//	float lerpedScale = std::lerp(0.1f, 1.0f, t);
	//	Vector3 scale = { lerpedScale, lerpedScale, lerpedScale };
	//	preSpawnedPlayer_->SetScale(scale);
	//}

	//// タイマーが0以下になったらプレイヤーを有効にする
	//if (rotationTimer_ <= 0.0f)
	//{
	//	playerSpawn_[playerSpawnIndex_]->ParticleStop();
	//	playerSpawn_[playerSpawnIndex_]->IsShaking(false);

	//	if (howManyBoogie_ < kMaxSpawnNum && preSpawnedPlayer_)
	//	{
	//		rotationTimer_ = rotation_;

	//		preSpawnedPlayer_->SetScale({ 1.0f, 1.0f, 1.0f });
	//		preSpawnedPlayer_->IsMoveable(true);

	//		players_.push_back(std::move(preSpawnedPlayer_));
	//		howManyBoogie_++;
	//		playerNum_++;

	//		playerSpawnIndex_++;
	//		if (playerSpawnIndex_ > playerSpawnNum_ - 1)
	//		{
	//			playerSpawnIndex_ = 0;
	//		}
	//	}
	//}
}

std::unique_ptr<Player> PlayerSpawnManager::TryExportSpawnedPlayer()
{
	// 出現が完了しており、仮プレイヤーが存在すれば移動で返す
	if (preSpawnedPlayer_ && rotationTimer_ == rotation_)
	{
		auto player = std::move(preSpawnedPlayer_);
		return player;
	}

	return nullptr;
}
