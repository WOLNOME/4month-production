#include "RemainingSpawnNum.h"
#include "TextWrite.h"

void RemainingSpawnNum::Initialize(uint32_t kMaxSpawnNum)
{
	// 残り出現数UIテキスト
	remainingSpawnNumText_ = std::make_unique<TextWrite>();
	remainingSpawnNumText_->Initialize("REMAINING");
	remainingSpawnNumText_->SetParam({ 10.0f, 100.0f }, Font::GenEiPOPle, 40.0f, { 1, 1, 1, 1 });
	remainingSpawnNumText_->SetEdgeParam({ 0, 0, 0, 1 }, 3.0f, 0.0f, true);
	// 残りの出現数テキスト
	numText_ = std::make_unique<TextWrite>();
	numText_->Initialize("NUM");
	numText_->SetParam({ 50.0f, 150.0f }, Font::GenEiPOPle, 70.0f, { 0.9f, 0.85f, 0.13f, 1 });
	numText_->SetEdgeParam({ 0, 0, 0, 1 }, 3.0f, 0.0f, true);
	// 値のテキスト
	valueText_ = std::make_unique<TextWrite>();
	valueText_->Initialize("value");
	valueText_->SetParam({ 160.0f, 185.0f }, Font::GenEiPOPle, 40.0f, { 1, 1, 1, 1 });
	valueText_->SetEdgeParam({ 0, 0, 0, 1 }, 3.0f, 0.0f, true);

	MaxSpawnNum_ = kMaxSpawnNum; // 最大出現数を設定
}

void RemainingSpawnNum::Update(uint32_t howManyBoogie)
{
	// 残り出現数を計算
	remainingBoogie_ = MaxSpawnNum_ - howManyBoogie;
	if (remainingBoogie_ < 0) {
		remainingBoogie_ = 0; // 負の値にならないように制限
	}
}

void RemainingSpawnNum::DebugWithImGui()
{
	//UIテキスト用のImGui
	valueText_->DebugWithImGui();
}

void RemainingSpawnNum::TextDraw()
{
	//スペースUIテキスト
	remainingSpawnNumText_->WriteText(L"残り出現数");
	// 残りの出現数テキスト
	numText_->WriteText(std::to_wstring(remainingBoogie_));
	// 値のテキスト
	valueText_->WriteText(L"体");
}
