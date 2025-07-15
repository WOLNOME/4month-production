#pragma once
#include <memory>
class Sprite;
#include <Vector2.h>
#include <vector>

class Interval
{
public:

	void Initialize(std::vector<uint32_t>& textureHandleIntervalNum);

	void SpriteUpdate();

	void UpdateIntervalNum();

	void Draw();

	//インターバルの終了
	bool IsEndInterval() const { return gameStartDelayTimer_ <= 0.0f; }

	bool GetHasPreUpdated() const { return hasPreUpdated_; }

	void SetHasPreUpdated(bool hasPreUpdated) { hasPreUpdated_ = hasPreUpdated; }

private:
	std::vector<std::unique_ptr<Sprite>> spritesIntervalNum_;

	// ゲーム開始のインターバル
	float gameStartDelayTimer_ = 3.0f;
	bool hasPreUpdated_ = false;

	Vector2 numSize_ = { 320.0f, 480.0f };
	uint32_t numSizeTimer_ = 0;
};

