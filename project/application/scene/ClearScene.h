#pragma once

#include "BaseScene.h"
#include "GameCamera.h"
#include "application/objects/Player/Player.h"
#include "application/objects/Field/Field.h"
#include "application/objects/Skydome/Skydome.h"

class ClearScene : public BaseScene
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

	// カメラ演出
	void CameraEffect();

	// プレイヤーのモーション
	void PlayerMotion();

	// クリアテクスチャの描画
	void ClearTextureUpdate();

private://メンバ変数
	//インプット
	Input* input_ = nullptr;

	//カメラ
	std::unique_ptr<GameCamera> camera;
	Vector3 cameraTranslate = { 0.0f,7.5f,-22.0f };
	Vector3 cameraRotate = { 0.25f,0.0f,0.0f };

	//スプライト
	uint32_t textureHandleClear_ = 0u;
	std::unique_ptr<Sprite> spriteClear_ = nullptr;
	//スペースUIテキスト
	std::unique_ptr<TextWrite> spriteUI_SPACE_ = nullptr;

	//スカイドーム
	std::unique_ptr<Skydome> skydome_ = nullptr;

	//フィールド
	std::unique_ptr<Field> field_ = nullptr;

	// プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

	std::unique_ptr<Audio> bgm_ = nullptr;
	std::unique_ptr<Audio> tapSE_ = nullptr;

	// カメラアニメーション中かどうか
	bool isCameraAnimating_ = true; 
	// アニメーションの進行度 (0.0f ～ 1.0f)
	float cameraAnimationProgress_ = 0.0f;
	// カメラアニメーションのフェーズ (0: 左から正面, 1: 右から正面)
	int cameraAnimationPhase_ = 0; 
	// プレイヤーの位置
	Vector3 playerPosition_ = { 0.0f, 1.3f, 0.0f };

	// プレイヤーのジャンプ進行度 (0.0f ～ 1.0f)
	float playerJumpProgress_ = 0.0f; 
	// プレイヤーが上昇中かどうか
	bool isPlayerJumpingUp_ = true; 
	// ジャンプの高さ
	float playerJumpHeight_ = 2.0f;
	// ジャンプの速度
	float playerJumpSpeed_ = 0.01f; 
	// 元に戻す処理中かどうかを判定するフラグ
	bool isReturning_ = false;
	// 元に戻す進行度 (0.0f ～ 1.0f)
	float progress_ = 0.0f;

	// スプライトアニメーション中かどうか
	bool isSpriteAnimating_ = false; 
	// スプライトアニメーションの進行度
	float spriteAnimationProgress_ = 0.0f;
	// スプライト処理が完了したかどうか
	bool isSpriteAnimationCompleted_ = false;

};

