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

private:

	void SetupCsvFilePath() override;

	void UpdateIntervalNum() override;

	void StartInterVal() override;

private://メンバ変数

	//チュートリアルシステム
	std::unique_ptr<TutorialSystem> tutorialSystem_ = nullptr;

};

