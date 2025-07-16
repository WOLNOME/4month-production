#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Audio.h"
#include "Vector2.h"
#include "Input.h"
#include "LineDrawer.h"
#include "GameCamera.h"

#include <cstdint>
#include <memory>

#include "../../application/objects/Skydome/Skydome.h"
#include "../../application/objects/Field/StageSelectObject.h"
#include "../../application/objects/GameObject/FallingObject.h"

class StageSelectScene : public BaseScene {
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

	// ステージ選択
	void SelectStageOperator();

private://メンバ変数

	//インプット
	Input* input_ = nullptr;

	std::unique_ptr<GameCamera> camera_ = nullptr;

	Vector3 cameraTranslate = { 0.0f,4.0f,-10.0f };
	Vector3 cameraRotate = { 0.4f,0.0f,0.0f };

	// 天球
	std::unique_ptr<Skydome> skydome_ = nullptr;
	// オブジェクトまとめ
	std::vector<std::unique_ptr<StageSelectObject>> selectObjects_{};
	std::unique_ptr<FallingObject> fallingObject_ = nullptr;

	const uint32_t stageNum_ = 6;
	static uint32_t setStage_;
	// ステージ選択番号描画用変数
	uint32_t drawSelectNum_ = setStage_;

	bool isTransitionTitle_ = false; // タイトルシーンへの遷移フラグ

	// ステージ選択スプライト
	uint32_t textureHandleUI_A_ = 0u;
	std::unique_ptr<Sprite> spriteUI_A_ = nullptr;

	uint32_t textureHandleUI_D_ = 0u;
	std::unique_ptr<Sprite> spriteUI_D_ = nullptr;

	uint32_t textureHandleUI_SELECT = 0u;
	std::unique_ptr<Sprite> spriteUI_SELECT_;

	//ステージナンバーUIテキスト
	std::vector<std::unique_ptr<TextWrite>> selectNumText_;
	Vector2 textPos_{};

	//スペースUIテキスト
	std::unique_ptr<TextWrite> spaceText_ = nullptr;


	//パーティクル情報
	std::unique_ptr<Particle> particle_ = nullptr;
	//bgm
	std::unique_ptr<Audio> bgm_ = nullptr;
	//効果音
	std::unique_ptr<Audio> selectSE_ = nullptr;
	std::unique_ptr<Audio> tapSE_ = nullptr;
};

