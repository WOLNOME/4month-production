#include "TutorialSystem.h"
#include "TextureManager.h"
#include "RandomStringUtil.h"

TutorialSystem::~TutorialSystem() {
}

void TutorialSystem::Initialize() {
	//インプット
	input_ = Input::GetInstance();
	//テクスチャマネージャー
	TextureManager* textureManager = TextureManager::GetInstance();


	//バックスプライト
	thBack_ = textureManager->LoadTexture("TUTORIAL/Tutorial_back.png");
	back_ = std::make_unique<Sprite>();
	back_->Initialize(thBack_);

	//テキスト
	for (int i = 0; i < 3; i++) {
		text_[i] = std::make_unique<TextWrite>();
		text_[i]->Initialize("StartText" + RandomStringUtil::GenerateRandomString(4));
		text_[i]->SetParam({ 200.0f,120.0f + (i * 90.0f) }, Font::UDDegitalNK_B, 80.0f, { 1,1,1,1 });
		text_[i]->SetEdgeParam({ 0,0,0,1 }, 8.0f, { 0,0 }, true);
	}

	//次のページへUI
	npText_ = L"[TAB]で次へ";
	nextPageText_ = std::make_unique<TextWrite>();
	nextPageText_->Initialize("NPText");
	nextPageText_->SetParam({ 465.0f,550.0f }, Font::UDDegitalNK_R, 60.0f, { 1,1,1,1 });
	nextPageText_->SetEdgeParam({ 0,0,0,1 }, 5.0f, { 0,0 }, true);

	//メニューテキストUI
	menuText_ = std::make_unique<TextWrite>();
	menuText_->Initialize("MenuText");
	menuText_->SetParam({ 400.0f,25.0f }, Font::UDDegitalNK_B, 70.0f, { 1,1,1,1 });
	menuText_->SetEdgeParam({ 0,0,0,1 }, 6.0f, { 0,0 }, true);

	//現在のメニュー
	currentMenu_ = Menu::Start;
	//タイムストップ
	isTimeStop_ = false;
	//残機表示
	isZankiDisplay = false;

	///=============================///
	/// スタート説明メンバ変数初期化
	///=============================///

	//タイマー
	startTimer_ = 0;
	//ページ
	startPage_ = 0;

	///=============================///
	/// ルール説明メンバ変数初期化
	///=============================///

	//ページ
	rulePage_ = 0;



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

	//スプライトの更新
	back_->Update();

#ifdef _DEBUG

#endif // _DEBUG

}

void TutorialSystem::DrawSprite() {
	switch (currentMenu_) {
	case Menu::Start:
		//ページ[2]の時はESCを注目させるテクスチャに切り替える
		if (startPage_ == 2) {
			back_->SetTextureHandle(TextureManager::GetInstance()->LoadTexture("TUTORIAL/Tutorial_ESC.png"));
		}
		else {
			back_->SetTextureHandle(TextureManager::GetInstance()->LoadTexture("TUTORIAL/Tutorial_back.png"));
		}
		back_->Draw();
		break;
	case Menu::Rule:
		back_->SetTextureHandle(TextureManager::GetInstance()->LoadTexture("TUTORIAL/Tutorial_back.png"));
		back_->Draw();
		break;
	case Menu::Move:
		//ページ[0]の時はWASDを注目させるテクスチャに切り替える
		if (movePage_ == 0) {
			back_->SetTextureHandle(TextureManager::GetInstance()->LoadTexture("TUTORIAL/Tutorial_Move.png"));
		}
		else {
			back_->SetTextureHandle(TextureManager::GetInstance()->LoadTexture("TUTORIAL/Tutorial_back.png"));
		}

		back_->Draw();

		break;
	case Menu::Zanki:
		//ページ[1]の時は残機表示を注目させるテクスチャに切り替える
		if (zankiPage_ == 1) {
			back_->SetTextureHandle(TextureManager::GetInstance()->LoadTexture("TUTORIAL/Tutorial_Zanki.png"));
		}
		else {
			back_->SetTextureHandle(TextureManager::GetInstance()->LoadTexture("TUTORIAL/Tutorial_back.png"));
		}

		back_->Draw();

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
	std::wstring menuText;
	std::array<std::vector<std::wstring>, 3> outputText;
	switch (currentMenu_) {
	case Menu::Start:
		//メニューのテキスト
		menuText = L"~はじめに~";
		//ページによって出力する文字列を変える
		outputText[0].push_back(L"チュートリアルへようこそ");
		outputText[1].push_back(L"");
		outputText[2].push_back(L"");

		outputText[0].push_back(L"このステージでは");
		outputText[1].push_back(L"ゲームの練習ができます。");
		outputText[2].push_back(L"");

		outputText[0].push_back(L"終了したい時は");
		outputText[1].push_back(L"[ESCAPE]からいつでも");
		outputText[2].push_back(L"終了できます。");

		//メニューテキスト
		menuText_->WriteText(menuText);

		//説明テキスト
		for (int i = 0; i < 3; i++) {
			text_[i]->WriteText(outputText[i][startPage_]);
		}

		//次のページへ
		nextPageText_->WriteText(npText_);
		break;
	case Menu::Rule:
		//メニューのテキスト
		menuText = L"~ルール説明~";
		//ページによって出力する文字列を変える
		outputText[0].push_back(L"ステージ上にいる敵を");
		outputText[1].push_back(L"全員場外に落とすと");
		outputText[2].push_back(L"ゲームクリアです。");

		outputText[0].push_back(L"自機は最初1体しか");
		outputText[1].push_back(L"いませんが、時間が経つと");
		outputText[2].push_back(L"どんどん増えていきます。");

		outputText[0].push_back(L"増える場所は決まっていて");
		outputText[1].push_back(L"ステージ上の黄色い目印から");
		outputText[2].push_back(L"出てきます。");

		outputText[0].push_back(L"ただし、自機が全て");
		outputText[1].push_back(L"場外に落ちてしまったら");
		outputText[2].push_back(L"ゲームオーバーです");

		//メニューUI描画
		menuText_->WriteText(menuText);

		//説明テキスト
		for (int i = 0; i < 3; i++) {
			text_[i]->WriteText(outputText[i][rulePage_]);
		}

		//次のページへ
		nextPageText_->WriteText(npText_);

		break;
	case Menu::Move:
		//メニューのテキスト
		menuText = L"~移動の仕方~";
		//ページによって出力する文字列を変える
		outputText[0].push_back(L"移動方法は");
		outputText[1].push_back(L"[W][A][S][D]キーで");
		outputText[2].push_back(L"上下左右に移動できます。");

		//メニューUI描画
		menuText_->WriteText(menuText);

		//説明テキスト
		for (int i = 0; i < 3; i++) {
			text_[i]->WriteText(outputText[i][movePage_]);
		}

		//次のページへ
		nextPageText_->WriteText(npText_);
		break;
	case Menu::Zanki:
		//メニューのテキスト
		menuText = L"~残機について~";

		//ページによって出力する文字列を変える
		outputText[0].push_back(L"自機が無限に増えることは");
		outputText[1].push_back(L"ありません");
		outputText[2].push_back(L"増える残機が決まっています");

		outputText[0].push_back(L"ここで確認できるため");
		outputText[1].push_back(L"残機に気を付けて");
		outputText[2].push_back(L"クリアを目指しましょう。");

		//メニューUI描画
		menuText_->WriteText(menuText);

		//説明テキスト
		for (int i = 0; i < 3; i++) {
			text_[i]->WriteText(outputText[i][zankiPage_]);
		}

		//次のページへ
		nextPageText_->WriteText(npText_);

		break;
	case Menu::Attack:
		//メニューのテキスト
		menuText = L"~攻撃の仕方~";

		//メニューUI描画
		menuText_->WriteText(menuText);

		break;
	case Menu::Gauge:
		//メニューのテキスト
		menuText = L"~ゲージについて~";

		//メニューUI描画
		menuText_->WriteText(menuText);

		break;
	case Menu::Battle:
		//メニューのテキスト
		menuText = L"";

		//メニューUI描画
		menuText_->WriteText(menuText);

		break;
	case Menu::ReturnStart:
		//メニューのテキスト
		menuText = L"";

		//メニューUI描画
		menuText_->WriteText(menuText);

		break;
	default:
		break;
	}
}

void TutorialSystem::Start() {
	//TABキーで次へ進む
	if (input_->TriggerKey(DIK_TAB)) {
		if (startPage_ + 1 < kNumMaxStartPage_) {
			startPage_++;
		}
		else {
			currentMenu_ = Menu::Rule;
		}
	}

	//5f後にタイムストップ
	startTimer_++;
	if (startTimer_ > 5) {
		isTimeStop_ = true;
	}

}

void TutorialSystem::Rule() {
	//TABキーで次へ進む
	if (input_->TriggerKey(DIK_TAB)) {
		if (rulePage_ + 1 < kNumMaxRulePage_) {
			rulePage_++;
		}
		else {
			currentMenu_ = Menu::Move;
		}
	}
}

void TutorialSystem::Move() {
	//TABキーで次へ進む
	if (input_->TriggerKey(DIK_TAB)) {
		if (movePage_ + 1 < kNumMaxMovePage_) {
			movePage_++;
		}
		else {
			currentMenu_ = Menu::Zanki;
		}
	}
}

void TutorialSystem::Zanki() {
	//TABキーで次へ進む
	if (input_->TriggerKey(DIK_TAB)) {
		if (zankiPage_ + 1 < kNumMaxZankiPage_) {
			zankiPage_++;
			//[1]ページで残機を表示させる
			if (zankiPage_ == 1) {
				isZankiDisplay = true;
			}
			else {
				isZankiDisplay = false;
			}
		}
		else {
			currentMenu_ = Menu::Attack;
			isZankiDisplay = false;
		}
	}
}

void TutorialSystem::Attack() {
}

void TutorialSystem::Gauge() {
}

void TutorialSystem::Battle() {
}

void TutorialSystem::ReturnStart() {
}
