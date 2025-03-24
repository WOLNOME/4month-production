#include "Skydome.h"
#include "TextureManager.h"

void Skydome::Initialize() {
	//テクスチャの読み込み
	textureHandle_ = TextureManager::GetInstance()->LoadTexture("sky.png");
	//ワールドトランスフォームの初期化
	wtSkydome_.Initialize();
	//モデルの生成
	skydome_ = std::make_unique<Object3d>();
	skydome_->InitializeModel("skydome");

}

void Skydome::Update() {
}

void Skydome::Draw(const BaseCamera& camera) {
	//モデルの描画
	skydome_->Draw(wtSkydome_, camera, nullptr, textureHandle_);
}
