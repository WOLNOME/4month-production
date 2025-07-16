#include "Charge.h"
#include "Sprite.h"
#include "TextureManager.h"
#include <algorithm>
#include "Input.h"

//アプリケーションインクルード
#include "../../application/objects/Player/Player.h"

void Charge::Initialize(uint32_t textureHandle)
{

	input_ = Input::GetInstance();

	// チャージUI
	spriteUI_Charge_ = std::make_unique<Sprite>();
	spriteUI_Charge_->SetPosition({ 1085.0f, 600.0f });
	spriteUI_Charge_->Initialize(textureHandle);

	spriteUI_ChargeGage_ = std::make_unique<Sprite>();
	spriteUI_ChargeGage_->Initialize(textureHandle);
	spriteUI_ChargeGage_->SetPosition({ 1085.0f, 600.0f });
	spriteUI_ChargeGage_->SetSize({ 180.0f, 50.0f });
	spriteUI_ChargeGage_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
}

void Charge::Update()
{
	// チャージの大きさに応じてスプライトのサイズを変更
	// チャージの割合を計算
	float chargeScale = chargeValue_ / chargeMax_;
	chargeScale = std::clamp(chargeScale, 0.0f, 1.0f); // 0.0f～1.0fの範囲に制限

	// チャージの割合に応じてテクスチャのXサイズを変更
	float newWidth = 180.0f * chargeScale; // 0から120まで拡大
	spriteUI_Charge_->SetSize({ newWidth, 50.0f }); // 横方向のサイズを変更

	spriteUI_Charge_->Update();
	spriteUI_ChargeGage_->Update();
}

void Charge::Draw()
{
	spriteUI_ChargeGage_->Draw();
	spriteUI_Charge_->Draw();
}

void Charge::playerTackleCharge(bool isPlayerAlive, std::vector<std::unique_ptr<Player>>& players_)
{
	if (!isPlayerAlive)
	{
		return; // プレイヤーが生存していない場合はチャージを行わない
	}
	// チャージが最大でないとき
	if (chargeValue_ < chargeMax_)
	{
		chargeValue_ += 1.0f;
	}
	// チャージが最大値に達したら
	else
	{
		if (!isChargeMax_)
		{
			isChargeMax_ = true;
		}

		if (input_->TriggerKey(DIK_SPACE) && isChargeMax_) {

			for (std::unique_ptr<Player>& player : players_)
			{
				if (!player->IsAttack())
				{
					continue;
				}
				isChargeMax_ = false;
				chargeValue_ = 0.0f;

				return;
			}

		}
	}

}
