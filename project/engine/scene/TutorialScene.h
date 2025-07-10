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

	// カメラのセットアップ
	void SetupCamera() override;

	//プレイヤーのスポーン位置のセットアップ
	void SetupPlayerSpawnPositions() override;

	//エネミーマネージャーのセットアップ
	void SetupEnemyManager() override;

	//フィールドのセットアップ
	void SetupField() override;

	//障害物の生成
	void CreateObstacles() override;

	//跳ね返る障害物の生成
	void CreateBumpers() override;

	//氷の床の生成
	void CreateIceFloors() override;

private://メンバ変数

	//チュートリアルシステム
	std::unique_ptr<TutorialSystem> tutorialSystem_ = nullptr;

};

