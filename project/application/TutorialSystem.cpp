#include "TutorialSystem.h"
#include "TextureManager.h"

TutorialSystem::~TutorialSystem() {
}

void TutorialSystem::Initialize() {
	//インプット
	input_ = Input::GetInstance();
	//テクスチャマネージャー
	TextureManager* textureManager = TextureManager::GetInstance();

	//現在のメニュー
	currentMenu_ = Menu::Start;
	//タイムストップ
	isTimeStop_ = false;
	//残機表示
	isZankiDisplay = false;

	///=============================///
	/// スタート説明メンバ変数初期化
	///=============================///

	//バックスプライト
	thStartBack_ = textureManager->LoadTexture("TUTORIAL/Tutorial_back.png");
	startBack_ = std::make_unique<Sprite>();
	startBack_->Initialize(thStartBack_);
	
	//テキスト
	startText_ = std::make_unique<TextWrite>();
	startText_->Initialize("startText");
	startText_->SetParam({ 0,0 }, Font::UDDegitalNK_B, 32.0f, { 1,1,1,1 });
	startText_->SetEdgeParam({ 0,0,0,1 }, 5.0f, { 0,0 }, true);

	//タイマー
	int startTimer_ = 0;


	///=============================///
	/// ルール説明メンバ変数初期化
	///=============================///


	///=============================///
	/// 移動説明メンバ変数初期化
	///=============================///


	///=============================///
	/// 残機説明メンバ変数初期化
	///=============================///


	///=============================///
	/// 攻撃説明メンバ変数初期化
	///=============================///


	///=============================///
	/// ゲージ説明メンバ変数初期化
	///=============================///


	///=============================///
	/// バトルフェーズメンバ変数初期化
	///=============================///


	///=============================///
	/// もう一度聞くかメンバ変数初期化
	///=============================///


}

void TutorialSystem::Update() {
	switch (currentMenu_) {
	case Menu::Start:
		Start();
		break;
	case Menu::Rule:
		Rule();
		break;
	case Menu::Move:
		Move();
		break;
	case Menu::Zanki:
		Zanki();
		break;
	case Menu::Attack:
		Attack();
		break;
	case Menu::Gauge:
		Gauge();
		break;
	case Menu::Battle:
		Battle();
		break;
	case Menu::ReturnStart:
		ReturnStart();
		break;
	default:
		break;
	}

#ifdef _DEBUG
	//テキストのImGuiデバッグ
	startText_->DebugWithImGui();


#endif // _DEBUG

}

void TutorialSystem::DrawSprite() {
	switch (currentMenu_) {
	case Menu::Start:
		startBack_->Draw();
		break;
	case Menu::Rule:
		break;
	case Menu::Move:
		break;
	case Menu::Zanki:
		break;
	case Menu::Attack:
		break;
	case Menu::Gauge:
		break;
	case Menu::Battle:
		break;
	case Menu::ReturnStart:
		break;
	case Menu::kNumMenu:
		break;
	default:
		break;
	}
}

void TutorialSystem::WriteText() {
	switch (currentMenu_) {
	case Menu::Start:
		startText_->WriteText(L"チュートリアルへようこそ。");
		break;
	case Menu::Rule:
		break;
	case Menu::Move:
		break;
	case Menu::Zanki:
		break;
	case Menu::Attack:
		break;
	case Menu::Gauge:
		break;
	case Menu::Battle:
		break;
	case Menu::ReturnStart:
		break;
	case Menu::kNumMenu:
		break;
	default:
		break;
	}
}

void TutorialSystem::Start() {
	//5f後にタイムストップ
	startTimer_++;
	if (startTimer_ > 5) {
		isTimeStop_ = true;
	}
	//スプライトの更新
	startBack_->Update();
}

void TutorialSystem::Rule() {
}

void TutorialSystem::Move() {
}

void TutorialSystem::Zanki() {
}

void TutorialSystem::Attack() {
}

void TutorialSystem::Gauge() {
}

void TutorialSystem::Battle() {
}

void TutorialSystem::ReturnStart() {
}
