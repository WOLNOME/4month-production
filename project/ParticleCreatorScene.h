#pragma once
#include "BaseScene.h"
#include "json.hpp"

class ParticleCreatorScene : public BaseScene {
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
	// 編集するパーティクル
	std::unique_ptr<Particle> partile_ = nullptr;
	



};

