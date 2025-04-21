#pragma once
#include "Input.h"
#include "SceneManager.h"
#include "Sprite.h"
#include "TextWrite.h"
#include <array>

class PauseSystem {
private:
	enum class MenuState {
		Continue,			//続行
		HowToPlay,			//遊び方
		Quit,				//終了
		kMaxNumMenuState,	//メニューの数
	};
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
	//ポーズ中のメニュー操作
	void MenuAlgorithm();
private:
	// 入力
	Input* input_ = nullptr;
	//シーンマネージャー
	SceneManager* sceneManager_ = nullptr;


	// スプライト
	uint32_t textureHandleCurtain;
	std::unique_ptr<Sprite> spriteCurtain = nullptr;										//薄暗いスプライト
	uint32_t textureHandleMenu;
	std::array<std::unique_ptr<Sprite>, (int)MenuState::kMaxNumMenuState> spriteMenu_;		//メニューのスプライト

	//テキスト
	std::unique_ptr<TextWrite> textPause_ = nullptr;										//一時停止中テキスト
	std::array<std::unique_ptr<TextWrite>,(int)MenuState::kMaxNumMenuState> textMenu_;		//メニューのテキスト
private:
	bool isPause_ = false;		//ポーズ中かどうか
	MenuState menuState_ = MenuState::Continue;	//メニューの状態

};

