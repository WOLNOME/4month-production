#pragma once
#include "BaseScene.h"
#include "TextWrite.h"

class TitleScene : public BaseScene {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 終了時
	/// </summary>
	void Finalize() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;
	/// <summary>
	/// テキスト描画
	/// </summary>
	void TextDraw() override;

private://メンバ変数
	Input* input_ = nullptr;

	//タイトルスプライト
	uint32_t textureHandleTitle_ = 0u;
	std::unique_ptr<Sprite> spriteTitle_ = nullptr;

	//テキスト
	std::unique_ptr<TextWrite> text1_;
	std::unique_ptr<TextWrite> text2_;

	//タイマー
	int timer_ = 0;
};

