#pragma once

#include "BaseScene.h"
#include "DevelopCamera.h"
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

	void EyeClose();

private://メンバ変数

	//インプット
	Input* input_ = nullptr;

	//開発用カメラ
	std::unique_ptr<DevelopCamera> camera;
	Vector3 cameraTranslate = { 0.0f, 1.0f, -10.0f };

	//スプライト
	uint32_t textureHandleFall1_ = 0u;
	std::unique_ptr<Sprite> spriteFall1_ = nullptr;
	uint32_t textureHandleFall2_ = 0u;
	std::unique_ptr<Sprite> spriteFall2_ = nullptr;

	//スペースUIテキスト
	std::unique_ptr<TextWrite> spriteUI_SPACE_ = nullptr;

	//スカイドーム
	std::unique_ptr<Skydome> skydome_ = nullptr;

	//フィールド
	std::unique_ptr<Field> field_ = nullptr;

	//bgm
	std::unique_ptr<Audio> bgm_ = nullptr;

	float spriteFall1Y_ = -720.0f; // spriteFall1_ の Y 座標
	float spriteFall2Y_ = 720.0f; // spriteFall2_ の Y 座標（初期位置は画面下）
	float fallSpeed_ = 20.0f; // 瞳を閉じる速度（初期は速い）
	int closeState_ = 0; // 0: 初期状態, 1: 速く閉じる, 2: 開く, 3: ゆっくり閉じる
	bool isAnimationComplete_ = false; // 演出が完了したかどうか
	bool isAnimationStert_ = false; // 演出が開始したかどうか

};

