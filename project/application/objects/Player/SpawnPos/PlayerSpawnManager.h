#pragma once

#include <vector>

#include "../../application/objects/Player/SpawnPos/SpawnPos.h"
#include "../../application/objects/Player/Player.h"

class Charge;
class GameCamera;
class PlayerSpawnManager
{
public:

	// 初期化
	void Initialize();

	// 終了
	void Finalize();

	// 更新
	void Update();

	// 描画
	void Draw(BaseCamera _camera);

	// ImGui描画
	void ImGuiDraw();

	//プレイヤースポーン処理
	void PlayerSpawnRotation();

	std::unique_ptr<Player> TryExportSpawnedPlayer();

public:	// セッター

	// プレイヤースポーン位置の取得
	void SetPlayerSpawnPosition(const std::vector<Vector3>& _positions) { playerSpawnPositions_ = _positions; }
	void SetPlayerSpawnPosition(const Vector3& _position) { playerSpawnPositions_.push_back(_position); }
	
	// 出現数の上限を設定
	void SetMaxSpawnNum(uint32_t _maxSpawnNum) { maxSpawnNum_ = _maxSpawnNum; }

	void SetCharge(Charge* charge) { charge_ = charge; }

	void SetGameCamera(GameCamera* _camera) { camera_ = _camera; }

public:	// ゲッター

	// プレイヤーの残り出現数を取得
	uint32_t GetHowManyBoogie() const { return howManyBoogie_; }

private:
	GameCamera* camera_ = nullptr;

	// プレイヤースポーン
	std::vector<std::unique_ptr<SpawnPos>> playerSpawn_{};

	std::unique_ptr<Player> preSpawnedPlayer_ = nullptr;

	// プレイヤースポーン位置の数(固定)
	const uint32_t playerSpawnNum_ = 3;
	// プレイヤースポーン(ローテーション用。どのポイントから出現させるか)
	uint32_t playerSpawnIndex_ = 0;
	// プレイヤースポーン位置
	std::vector<Vector3> playerSpawnPositions_{};
	// ローテーション間隔
	const float rotation_ = 240.0f;
	// ローテーション用タイマー
	float rotationTimer_ = rotation_;
	// 出現数上限
	uint32_t maxSpawnNum_ = 15;
	// 何体出たか
	uint32_t howManyBoogie_ = 0;


	Charge* charge_ = nullptr;

};

