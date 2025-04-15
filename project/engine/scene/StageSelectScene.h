#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Audio.h"
#include "Vector2.h"
#include "Input.h"
#include "LineDrawer.h"
#include "DevelopCamera.h"

#include <cstdint>
#include <memory>

#include "../../application/objects/Field/StageSelectObject.h"

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
	void StageSelect();

private://メンバ変数

	//インプット
	Input* input_ = nullptr;

	std::unique_ptr<BaseCamera> camera_ = nullptr;

	Vector3 cameraTranslate = { 0.0f,6.0f,-15.0f };
	Vector3 cameraRotate = { 0.4f,0.0f,0.0f };

	// モデル情報
	std::vector<std::unique_ptr<StageSelectObject>> selectObjects_{};

	const uint32_t stageNum_ = 5;
	uint32_t selectStage_ = 0;

	// ステージ選択スプライト
	uint32_t textureHandleSelect_ = 0u;
	std::unique_ptr<Sprite> spriteSelect_ = nullptr;

	uint32_t textureHandleUI_A_ = 0u;
	std::unique_ptr<Sprite> spriteUI_A_ = nullptr;

	uint32_t textureHandleUI_D_ = 0u;
	std::unique_ptr<Sprite> spriteUI_D_ = nullptr;

	std::vector<uint32_t>textureHandleSelectNum_;
	std::vector<std::unique_ptr<Sprite>> spriteSelectNum_;

	Vector2 spritePos_{};

	//パーティクル情報
	std::unique_ptr<Particle> particle_ = nullptr;
};

