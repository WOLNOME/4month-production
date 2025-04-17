#pragma once
#include "BaseScene.h"
#include "DevelopCamera.h"
#include "application/objects/Field/Field.h"
#include "application/objects/Player/Player.h"

class TitleScene : public BaseScene {
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

private: //メンバ関数
	void UpdateCutscene(); // カットシーンの更新
	void ResetCutscene(); // カットシーンのリセット
	bool CheckCollision(); // 当たり判定のチェック

private://メンバ変数
	Input* input_ = nullptr;
	//タイトルスプライト
	uint32_t textureHandleUI_ = 0u;
	std::unique_ptr<Sprite> spriteUI_ = nullptr;
	//開発用カメラ
	std::unique_ptr<DevelopCamera> camera;
	//タイトル
	std::unique_ptr<TextWrite> title_;
	//演出に使うプレイヤー
	std::unique_ptr<Object3d> player_ = nullptr;
	WorldTransform playerTransform_;
	//演出に使うエネミー
	std::unique_ptr<Object3d> enemy_ = nullptr;
	WorldTransform enemyTransform_;
	//フィールド
	std::unique_ptr<Field> field_ = nullptr;
	//当たり判定
	AppCollisionManager* appCollisionManager_ = nullptr;
	// 演出用の変数
	float cutsceneTime_ = 0.0f;
	const float cutsceneDuration_ = 1.0f; // カットシーンの長さ（秒）
	bool isCutscenePlaying_ = true;
	float waitTime_ = 0.0f; // 待機時間
	const float waitDuration_ = 3.0f; // 待機時間の長さ（秒）
	// 初速度
	const Vector3 initialPlayerVelocity_ = { 20.0f, 0.0f, 0.0f };
	const Vector3 initialEnemyVelocity_ = { 38.0f, 0.0f, 0.0f };
	Vector3 playerVelocity_ = { 0.0f, 0.0f, 0.0f };
	Vector3 playerAcceleration_ = { 0.5f, 0.0f, 0.0f }; // 加速度
	Vector3 enemyVelocity_ = { 0.0f, 0.0f, 0.0f };
	Vector3 enemyAcceleration_ = { 1.0f, 0.0f, 0.0f }; // 加速度
	float friction_ = 0.98f; // 摩擦係数
	enum class CutsceneState {
		PlayerRunningIn,
		PlayerPreparing,
		Hit,
		EnemyFlyingOut,
		ShowTitle,
		Wait,
		ZoomIn,
		Reset,
		ZoomOut,
	} cutsceneState_ = CutsceneState::ShowTitle;

	//タイトルロゴフェード用変数
	float logoAlpha_ = 0.0f; // タイトルロゴの透明度
	bool isFadingIn_ = true; // フェードイン中かどうか

	//カメラのズームイン・アウト用変数
	bool isZoomIn_ = false;
	bool isZoomOut_ = false;

	//ヒットエフェクト
	std::unique_ptr<Particle> hitEffect_;
	//背景のエフェクト
	std::unique_ptr<Particle> backgroundEffect_;
};

