#pragma once
#include <cstdint>
#include <memory>
#include <vector>
class Sprite;
class Input;
class Player;

class Charge
{
public:

	void Initialize(uint32_t textureHandle);

	void Update();

	void Draw();

	void playerTackleCharge(bool isPlayerAlive, std::vector<std::unique_ptr<Player>>& players_);

	bool* IsChargeMaxPtr() { return &isChargeMax_; }

	float GetChargeValue() { return chargeValue_; }

private:

	Input* input_ = nullptr;

	// チャージUI
	std::unique_ptr<Sprite> spriteUI_Charge_ = nullptr;
	std::unique_ptr<Sprite> spriteUI_ChargeGage_ = nullptr;

	// 攻撃チャージMax
	const float chargeMax_ = 80.0f;
	bool isChargeMax_ = false;
	// 攻撃チャージ
	float chargeValue_ = 0.0f;
};