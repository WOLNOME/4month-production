#pragma once
#include "Input.h"
#include "Sprite.h"
#include "TextWrite.h"

class PauseSystem {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 終了時
	/// </summary>
	void Finalize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void DrawSprite();
	/// <summary>
	/// テキスト描画
	/// </summary>
	void TextDraw();
	/// <summary>
	/// デバッグ
	/// </summary>
	void DebugWithImGui();

public:
	//ポーズ中かどうか
	bool GetIsPause() const { return isPause_; }
private:
	// 入力
	Input* input_ = nullptr;
	// スプライト
	uint32_t textureHandleCurtain;
	std::unique_ptr<Sprite> spriteCurtain = nullptr;
	
	//テキスト
	std::unique_ptr<TextWrite> textPause_ = nullptr;
private:
	bool isPause_ = false;		//ポーズ中かどうか


};

