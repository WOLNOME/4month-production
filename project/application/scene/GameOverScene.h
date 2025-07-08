#pragma once

#include "BaseScene.h"
#include "GameCamera.h"
#include "application/objects/Field/Field.h"
#include "application/objects/Skydome/Skydome.h"

class GameOverScene : public BaseScene
{
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

	void Close();

private://メンバ変数

	//インプット
	Input* input_ = nullptr;

	//カメラ
	std::unique_ptr<GameCamera> camera;
	Vector3 cameraTranslate = { 0.0f, 1.0f, -10.0f };

	//スプライト
	uint32_t textureHandleFall1_ = 0u;
	std::unique_ptr<Sprite> spriteFall1_ = nullptr;
	//uint32_t textureHandleFall2_ = 0u;
	//std::unique_ptr<Sprite> spriteFall2_ = nullptr;

	//スペースUIテキスト
	std::unique_ptr<TextWrite> spriteUI_SPACE_ = nullptr;

	//スカイドーム
	std::unique_ptr<Skydome> skydome_ = nullptr;

	//フィールド
	std::unique_ptr<Field> field_ = nullptr;
	Vector3 fieldScale_ = { 26.0f, 1.0f, 26.0f };

	//bgm
	std::unique_ptr<Audio> bgm_ = nullptr;
	//タップしたときの効果音
	std::unique_ptr<Audio> tapSE_ = nullptr;

	// スプライトの初期位置（画面外の上）
	float spriteFallY_ = -720.0f;
	// 降りてくる速度
	float fallSpeed_ = 30.0f;
	float bounceSpeed_ = -15.0f;
	float gravity_ = 1.0f;
	// 演出フラグ
	bool isAnimationComplete_ = false; 
	bool isAnimationStart_ = false;
	bool isBounce_ = false;
	int bounceCount_ = 0; // バウンド回数をカウント
	const int maxBounceCount_ = 3; // 最大バウンド回数


};

