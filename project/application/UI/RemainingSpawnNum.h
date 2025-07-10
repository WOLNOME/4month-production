#pragma once
#include <cstdint>
#include <memory>
class TextWrite; 

class RemainingSpawnNum
{
public:

	void Initialize(uint32_t kMaxSpawnNum);

	void Update(uint32_t howManyBoogie);

	void DebugWithImGui();

	void TextDraw();

private:
	// 残り出現数
	uint32_t remainingBoogie_ = 0;
	// 残りの出現数テキスト
	std::unique_ptr<TextWrite> remainingSpawnNumText_;
	std::unique_ptr<TextWrite> numText_;
	std::unique_ptr<TextWrite> valueText_;

	// 出現数上限
	uint32_t MaxSpawnNum_;
};

