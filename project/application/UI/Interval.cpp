#include "Interval.h"
#include "Sprite.h"

void Interval::Initialize(std::vector<uint32_t>& textureHandleIntervalNum)
{
	spritesIntervalNum_.resize(textureHandleIntervalNum.size());

	for (uint32_t i = 0; i < textureHandleIntervalNum.size(); ++i) {
		spritesIntervalNum_[i] = std::make_unique<Sprite>();
		spritesIntervalNum_[i]->Initialize(textureHandleIntervalNum[i]);
		spritesIntervalNum_[i]->SetAnchorPoint({ 0.5f, 0.5f });
		spritesIntervalNum_[i]->SetPosition({ 640.0f, 360.0f });
		spritesIntervalNum_[i]->SetSize({ 0.0f, 0.0f });
		spritesIntervalNum_[i]->SetColor({ 0.863f, 0.706f, 0.157f, 1.0f });
	}

}

void Interval::SpriteUpdate()
{
	for (std::unique_ptr<Sprite>& sprite : spritesIntervalNum_) {
		sprite->Update();
	}
}

void Interval::UpdateIntervalNum()
{
	// 数字のサイズを更新
	for (std::unique_ptr<Sprite>& sprite : spritesIntervalNum_) {
		sprite->SetSize(numSize_);
	}

	numSizeTimer_++;
	if (numSizeTimer_ > 60) {
		numSizeTimer_ = 0; // リピート or 一度きりなら終了
	}

	// 0〜π までの範囲に変換（0→最大→0）
	float t = static_cast<float>(numSizeTimer_) / 60.0f; // 0〜1
	float scale = std::sinf(t * 3.14159265f); // sin(0→π) = 0→1→0

	// 最大サイズに合わせてスケーリング
	numSize_.x = 320.0f * scale;
	numSize_.y = 480.0f * scale;

	// 3秒カウント
	gameStartDelayTimer_ -= 1.0f / 60.0f;
}

void Interval::Draw()
{
	size_t maxIndex = spritesIntervalNum_.size() - 1;

	for (size_t i = 0; i <= maxIndex; ++i) {
		float upper = static_cast<float>(maxIndex + 1 - i);
		float lower = upper - 1.0f;

		if (gameStartDelayTimer_ <= upper && gameStartDelayTimer_ > lower) {
			spritesIntervalNum_[maxIndex - i]->Draw();
			break;
		}
	}

}