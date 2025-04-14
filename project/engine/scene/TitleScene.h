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

private:
	void PlayerTackleCharge();

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
	std::unique_ptr<Player> player_ = nullptr;
	//フィールド
	std::unique_ptr<Field> field_ = nullptr;
	//当たり判定
	AppCollisionManager* appCollisionManager_ = nullptr;
	//
	float charge_ = 0.0f;
	//
	const float kChargeMax = 1.0f;
};

