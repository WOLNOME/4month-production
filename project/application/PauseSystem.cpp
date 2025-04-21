#include "PauseSystem.h"
#include "TextureManager.h"

void PauseSystem::Initialize() {
	//入力
	input_ = Input::GetInstance();
	//スプライト
	textureHandleCurtain = TextureManager::GetInstance()->LoadTexture("black.png");
	spriteCurtain = std::make_unique<Sprite>();
	spriteCurtain->Initialize(textureHandleCurtain);
	spriteCurtain->SetColor({ 1.0f,1.0f,1.0f,0.55f });

	//テキスト
	textPause_ = std::make_unique<TextWrite>();
	textPause_->Initialize("Pause");
	textPause_->SetParam({ 320.0f,50.0f }, Font::OnionScript, 80.0f, { 1,1,1,1 });
	textPause_->SetEdgeParam({ 0,0,0,1 }, 5.0f, 0.0f, true);
	
}

void PauseSystem::Finalize() {
}

void PauseSystem::Update() {
	//エスケープでポーズorポーズ解除
	if (input_->TriggerKey(DIK_ESCAPE)) {
		isPause_ = !isPause_;
	}
	//ポーズ中なら
	if (isPause_) {
		spriteCurtain->Update();
	}

}

void PauseSystem::DrawSprite() {
	//ポーズ中なら
	if (isPause_) {
		spriteCurtain->Draw();
	}
}

void PauseSystem::TextDraw() {
	//ポーズ中なら
	if (isPause_) {
		textPause_->WriteText(L"一時停止中");
	}
}

void PauseSystem::DebugWithImGui() {
#ifdef _DEBUG
	//テキスト
	textPause_->DebugWithImGui();
#endif // _DEBUG
}
