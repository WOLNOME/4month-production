#pragma once
#include "Input.h"
#include "Sprite.h"
#include "TextWrite.h"
#include <memory>

//チュートリアルメニュー
enum class Menu {
	Start,			//チュートリアルへようこそって感じのやつ
	Rule,			//ルールの説明
	Move,			//移動の仕方の説明
	Zanki,			//残機についての説明
	Attack,		//攻撃方法についての説明
	Gauge,			//攻撃ゲージについての説明
	Battle,		//実際のバトルフェーズ
	ReturnStart,	//もう一度聞くか

	kNumMenu,		//メニューの数
};
class TutorialSystem {

public:
	~TutorialSystem();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// スプライト描画
	/// </summary>
	void DrawSprite();
	/// <summary>
	/// テキスト描画
	/// </summary>
	void WriteText();

public://getter
	//現在のメニュー
	Menu GetCurrentMenu() const { return currentMenu_; }
	//タイムストップフラグ
	bool GetIsTimeStop() const { return isTimeStop_; }
	//残機表示フラグ
	bool GetIsZankiDisplay() const { return isZankiDisplay; }

private:
	//スタート処理
	void Start();
	//ルール説明処理
	void Rule();
	//移動説明処理
	void Move();
	//残機説明処理
	void Zanki();
	//攻撃説明処理
	void Attack();
	//ゲージ説明処理
	void Gauge();
	//バトルフェーズ処理
	void Battle();
	//もう一度聞くかの処理
	void ReturnStart();
private:
	//インプット
	Input* input_ = nullptr;

private://全体共有のメンバ変数
	//現在のメニュー
	Menu currentMenu_;
	//タイムストップ
	bool isTimeStop_ = false;
	//残機表示
	bool isZankiDisplay = false;

private://スタート説明のメンバ変数
	//バックスプライト
	int32_t thStartBack_ = EOF;
	std::unique_ptr<Sprite> startBack_ = nullptr;
	//テキスト
	std::unique_ptr<TextWrite> startText_ = nullptr;
	//タイマー
	int startTimer_;

private://ルール説明のメンバ変数

private://移動説明のメンバ変数

private://残機説明のメンバ変数

private://攻撃説明のメンバ変数

private://ゲージ説明のメンバ変数

private://バトルフェーズのメンバ変数

private://もう一度聞くかのメンバ変数

};

