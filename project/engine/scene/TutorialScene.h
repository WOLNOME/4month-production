#pragma once

#include "GamePlayScene.h"

// アプリケーションインクルード
#include "../../application/TutorialSystem.h"

class TutorialScene : public GamePlayScene {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
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

	//ステージ番号
	static uint32_t stageNum_;

private://メンバ変数

	//チュートリアルシステム
	std::unique_ptr<TutorialSystem> tutorialSystem_ = nullptr;

};

