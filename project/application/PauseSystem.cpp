#include "PauseSystem.h"
#include "WinApp.h"
#include "TextureManager.h"
#include "RandomStringUtil.h"

void PauseSystem::Initialize() {
	//入力
	input_ = Input::GetInstance();
	//シーンマネージャー
	sceneManager_ = SceneManager::GetInstance();

	//スプライト
	textureHandleCurtain = TextureManager::GetInstance()->LoadTexture("black.png");
	spriteCurtain = std::make_unique<Sprite>();
	spriteCurtain->Initialize(textureHandleCurtain);
	spriteCurtain->SetColor({ 1.0f,1.0f,1.0f,0.55f });

	textureHandleMenu = TextureManager::GetInstance()->LoadTexture("Flame.png");
	for (int i = 0; i < (int)MenuState::kMaxNumMenuState; i++) {
		spriteMenu_[i] = std::make_unique<Sprite>();
		spriteMenu_[i]->Initialize(textureHandleMenu);
		spriteMenu_[i]->SetAnchorPoint({ 0.5f,0.5f });
		spriteMenu_[i]->SetSize({ 400.0f, 80.0f });
		spriteMenu_[i]->SetColor({ 0.8f,0.8f,0.8f,1.0f });
		spriteMenu_[i]->SetPosition({ WinApp::kClientWidth / 2.0f, 300.0f + (i * 130) });
	}

	//テキスト
	textPause_ = std::make_unique<TextWrite>();
	textPause_->Initialize("Pause");
	textPause_->SetParam({ 415.0f,80.0f }, Font::Meiryo, 90.0f, { 1,1,1,1 });
	textPause_->SetEdgeParam({ 0,0,0,1 }, 8.0f, { 0.004f,-0.004f }, true);

	for (int i = 0; i < (int)MenuState::kMaxNumMenuState; i++) {
		textMenu_[i] = std::make_unique<TextWrite>();
		textMenu_[i]->Initialize("Menu" + RandomStringUtil::GenerateRandomString(3));
		textMenu_[i]->SetParam({ WinApp::kClientWidth / 2.0f - 60.0f, 260.0f + (i * 130) }, Font::Meiryo, 60.0f, { 1,1,1,1 });
		textMenu_[i]->SetEdgeParam({ 0,0,0,1 }, 8.0f, { 0.004f,-0.004f }, true);
		//遊び方テキスト蚤の処理
		if (i == (int)MenuState::HowToPlay) {
			textMenu_[i]->SetPosition({ textMenu_[i]->GetPosition().x - 30.0f,textMenu_[i]->GetPosition().y });
		}
	}

}

void PauseSystem::Finalize() {
}

void PauseSystem::Update() {
	//エスケープでポーズorポーズ解除
	if (input_->TriggerKey(DIK_ESCAPE)) {
		if (isPause_) {
			isPause_ = false; //ポーズ解除
		}
		else {
			isPause_ = true; //ポーズ
			//メニューの初期化
			menuState_ = MenuState::Continue;
			for (int i = 0; i < (int)MenuState::kMaxNumMenuState; i++) {
				spriteMenu_[i]->SetSize({ 400.0f, 80.0f });
				spriteMenu_[i]->SetColor({ 0.8f,0.8f,0.8f,1.0f });
				textMenu_[i]->SetSize(60.0f);
				textMenu_[i]->SetPosition({ WinApp::kClientWidth / 2.0f - 60.0f, 260.0f + (i * 130) });
				//遊び方テキスト蚤の処理
				if (i == (int)MenuState::HowToPlay) {
					textMenu_[i]->SetPosition({ textMenu_[i]->GetPosition().x - 30.0f,textMenu_[i]->GetPosition().y });
				}
			}
			spriteMenu_[0]->SetSize(spriteMenu_[0]->GetSize() * 1.2f);
			spriteMenu_[0]->SetColor({ 1.0f,1.0f,0.8f,1.0f });
			textMenu_[0]->SetSize(70.0f);
			textMenu_[0]->SetPosition({ textMenu_[0]->GetPosition().x - 15.0f,textMenu_[0]->GetPosition().y - 5.0f });
		}
	}
	//ポーズ中なら
	if (isPause_) {
		//メニュー操作
		MenuAlgorithm();
		//スプライトの更新
		spriteCurtain->Update();
		for (int i = 0; i < (int)MenuState::kMaxNumMenuState; i++) {
			spriteMenu_[i]->Update();
		}
	}

}

void PauseSystem::DrawSprite() {
	//ポーズ中なら
	if (isPause_) {
		//薄暗いスプライトを描画
		spriteCurtain->Draw();
		//メニューのスプライトを描画
		for (int i = 0; i < (int)MenuState::kMaxNumMenuState; i++) {
			spriteMenu_[i]->Draw();
		}
	}
}

void PauseSystem::TextDraw() {
	//ポーズ中なら
	if (isPause_) {
		//一時停止中
		textPause_->WriteText(L"一時停止中");
		//メニューのテキストを描画
		textMenu_[0]->WriteText(L"続行");
		textMenu_[1]->WriteText(L"遊び方");
		textMenu_[2]->WriteText(L"終了");
	}
}

void PauseSystem::DebugWithImGui() {
#ifdef _DEBUG
	//テキスト
	textPause_->DebugWithImGui();
#endif // _DEBUG
}

void PauseSystem::MenuAlgorithm() {
	//メニュー操作
	switch (menuState_) {
	case PauseSystem::MenuState::Continue:
		//操作
		if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S)) {
			//下に移動
			menuState_ = PauseSystem::MenuState::HowToPlay;
			//スプライトの編集
			spriteMenu_[0]->SetColor({ 0.8f,0.8f,0.8f,1.0f });
			spriteMenu_[0]->SetSize(spriteMenu_[0]->GetSize() / 1.2f);
			spriteMenu_[1]->SetColor({ 1.0f,1.0f,0.8f,1.0f });
			spriteMenu_[1]->SetSize(spriteMenu_[1]->GetSize() * 1.2f);
			//テキストの編集
			textMenu_[0]->SetSize(60.0f);
			textMenu_[0]->SetPosition({ textMenu_[0]->GetPosition().x + 15.0f,textMenu_[0]->GetPosition().y + 5.0f });
			textMenu_[1]->SetSize(70.0f);
			textMenu_[1]->SetPosition({ textMenu_[1]->GetPosition().x - 15.0f,textMenu_[1]->GetPosition().y - 5.0f });
		}
		else if (input_->TriggerKey(DIK_SPACE)) {
			//選択
			isPause_ = false; //ポーズ解除
		}

		break;
	case PauseSystem::MenuState::HowToPlay:
		//操作
		if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W)) {
			//上に移動
			menuState_ = PauseSystem::MenuState::Continue;
			//スプライトの編集
			spriteMenu_[1]->SetColor({ 0.8f,0.8f,0.8f,1.0f });
			spriteMenu_[1]->SetSize(spriteMenu_[1]->GetSize() / 1.2f);
			spriteMenu_[0]->SetColor({ 1.0f,1.0f,0.8f,1.0f });
			spriteMenu_[0]->SetSize(spriteMenu_[0]->GetSize() * 1.2f);
			//テキストの編集
			textMenu_[0]->SetSize(70.0f);
			textMenu_[0]->SetPosition({ textMenu_[0]->GetPosition().x - 15.0f,textMenu_[0]->GetPosition().y - 5.0f });
			textMenu_[1]->SetSize(60.0f);
			textMenu_[1]->SetPosition({ textMenu_[1]->GetPosition().x + 15.0f,textMenu_[1]->GetPosition().y + 5.0f });
		}
		else if (input_->TriggerKey(DIK_DOWN) || input_->TriggerKey(DIK_S)) {
			//下に移動
			menuState_ = PauseSystem::MenuState::Quit;
			//スプライトの編集
			spriteMenu_[1]->SetColor({ 0.8f,0.8f,0.8f,1.0f });
			spriteMenu_[1]->SetSize(spriteMenu_[1]->GetSize() / 1.2f);
			spriteMenu_[2]->SetColor({ 1.0f,1.0f,0.8f,1.0f });
			spriteMenu_[2]->SetSize(spriteMenu_[2]->GetSize() * 1.2f);
			//テキストの編集
			textMenu_[1]->SetSize(60.0f);
			textMenu_[1]->SetPosition({ textMenu_[1]->GetPosition().x + 15.0f,textMenu_[1]->GetPosition().y + 5.0f });
			textMenu_[2]->SetSize(70.0f);
			textMenu_[2]->SetPosition({ textMenu_[2]->GetPosition().x - 15.0f,textMenu_[2]->GetPosition().y - 5.0f });
		}
		else if (input_->TriggerKey(DIK_SPACE)) {

		}
		break;
	case PauseSystem::MenuState::Quit:
		//操作
		if (input_->TriggerKey(DIK_UP) || input_->TriggerKey(DIK_W)) {
			//上に移動
			menuState_ = PauseSystem::MenuState::HowToPlay;
			//スプライトの編集
			spriteMenu_[2]->SetColor({ 0.8f,0.8f,0.8f,1.0f });
			spriteMenu_[2]->SetSize(spriteMenu_[2]->GetSize() / 1.2f);
			spriteMenu_[1]->SetColor({ 1.0f,1.0f,0.8f,1.0f });
			spriteMenu_[1]->SetSize(spriteMenu_[1]->GetSize() * 1.2f);
			//テキストの編集
			textMenu_[1]->SetSize(70.0f);
			textMenu_[1]->SetPosition({ textMenu_[1]->GetPosition().x - 15.0f,textMenu_[1]->GetPosition().y - 5.0f });
			textMenu_[2]->SetSize(60.0f);
			textMenu_[2]->SetPosition({ textMenu_[2]->GetPosition().x + 15.0f,textMenu_[2]->GetPosition().y + 5.0f });
		}
		else if (input_->TriggerKey(DIK_SPACE)) {
			//セレクトシーンに戻る
			sceneManager_->SetNextScene("STAGESELECT");
		}
		break;
	case PauseSystem::MenuState::kMaxNumMenuState:
		//何もしない
		break;
	default:
		break;
	}
}
