#pragma once

#include "../../application/objects/Player/Player.h"

#include <Vector3.h>

class Charge;
class GameCamera;

class PlayerManager
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

	// プレイヤーの追加
	void AddPlayer(std::unique_ptr<Player> newPlayer);

	// トランスフォーム更新
	void UpdateTransform();

public: // セッター

	void IsMoveable(bool isMoveable);

	void SetSpawnPositions(const std::vector<Vector3>& positions);
	void SetSpawnPositions(const Vector3& position);

	void SetOnPlayerAdded(const std::function<void()>& callback);
	void SetOnPlayerRemoved(const std::function<void()>& callback);

	void SetCharge(Charge* charge) { charge_ = charge; }
	void SetGameCamera(GameCamera* _camera) { camera_ = _camera; }

public: // ゲッター

	// プレイヤーの取得
	std::vector<std::unique_ptr<Player>>& GetPlayers() { return players_; }

	// プレイヤーの位置を取得（indexで指定）
	Vector3 GetPlayerPosition(uint32_t index) const;

	// プレイヤーがフィールド上にいるかどうかを確認
	bool IsPlayerGrounded(uint32_t index) const;


private:
	//ゲームカメラ
	GameCamera* camera_ = nullptr;

	// プレイヤー
	std::vector<std::unique_ptr<Player>> players_{};

	// プレイヤースポーン位置
	std::vector<Vector3> playerSpawnPositions_{};

	std::function<void()> onPlayerAdded_;
	std::function<void()> onPlayerRemoved_;

	Charge* charge_ = nullptr;

};

