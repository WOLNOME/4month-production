#include "TextWrite.h"
#include "TextWriteManager.h"
#include "WinApp.h"
#include "ImGuiManager.h"

TextWrite::~TextWrite() {
	//マネージャーの登録を外す
	TextWriteManager::GetInstance()->CancelRegistration(name_);
}

void TextWrite::Initialize(const std::string& name) {
	name_ = name;

	//変数の初期化
	text_ = L"";
	font_ = Font::Meiryo;
	fontName_ = ReturnFontName(font_);
	color_ = D2D1::ColorF::White;
	position_ = { 0.0f,0.0f };
	width_ = 100.0f;
	height_ = 100.0f;

	//マネージャーに登録する
	TextWriteManager::GetInstance()->Registration(this);
}

void TextWrite::DebugWithImGui() {
#ifdef _DEBUG
	ImGui::Begin(("TextDebugger : " + name_).c_str());
	//テキストの座標
	ImGui::DragFloat2("position", &position_.x, 1.0f);
	SetPosition(position_);
	//フォント
	const char* fontNames[] = {
		"Meiryo",
		"YuGothic",
		"YuMincho",
		"UDDegitalN_B",
		"UDDegitalN_R",
		"UDDegitalNK_B",
		"UDDegitalNK_R",
		"UDDegitalNP_B",
		"UDDegitalNP_R",
		"OnionScript"
	};
	int fontIndex = static_cast<int>(font_);
	if (ImGui::Combo("Font", &fontIndex, fontNames, IM_ARRAYSIZE(fontNames))) {
		//フォントを変更
		SetFont(font_);
	}
	//テキストのサイズ
	ImGui::DragFloat("size", &size_, 0.1f, 10.0f, 80.0f);
	SetSize(size_);
	//テキストのカラー
	ImGui::ColorEdit4("color", &color_.x);
	SetColor(color_);


	ImGui::End();
#endif // _DEBUG
}

void TextWrite::SetParam(const Vector2& position, const Font& font, float size, const Vector4& color) {
	position_ = position;
	font_ = font;
	fontName_ = ReturnFontName(font_);
	size_ = size;
	color_ = color;

	//ポジションから幅と高さを計算
	width_ = WinApp::GetInstance()->kClientWidth - position_.x;
	height_ = WinApp::GetInstance()->kClientHeight - position_.y;

	//マネージャーに値をセット
	TextWriteManager::GetInstance()->EditSolidColorBrash(name_, color_);
	TextWriteManager::GetInstance()->EditTextFormat(name_, fontName_, size_);
}

void TextWrite::SetFont(const Font& font) {
	//フォントをセット
	font_ = font;
	//フォント名をセット
	fontName_ = ReturnFontName(font_);
	//フォント情報をマネージャーにセット
	TextWriteManager::GetInstance()->EditTextFormat(name_, fontName_, size_);
}

void TextWrite::SetSize(float size) {
	//サイズをセット
	size_ = size;
	//サイズ情報をマネージャーにセット
	TextWriteManager::GetInstance()->EditTextFormat(name_, fontName_, size_);
}

void TextWrite::SetColor(const Vector4& color) {
	//色をセット
	color_ = color;
	//色情報をマネージャーにセット
	TextWriteManager::GetInstance()->EditSolidColorBrash(name_, color_);
}

const std::wstring& TextWrite::ReturnFontName(const Font& font) {
	static const std::wstring meiryo = L"メイリオ";
	static const std::wstring yugothic = L"游ゴシック";
	static const std::wstring yumincho = L"游明朝";
	static const std::wstring udDegitalN_B = L"UD デジタル 教科書体 N-B";
	static const std::wstring udDegitalN_R = L"UD デジタル 教科書体 N-R";
	static const std::wstring udDegitalNK_B = L"UD デジタル 教科書体 NK-B";
	static const std::wstring udDegitalNK_R = L"UD デジタル 教科書体 NK-R";
	static const std::wstring udDegitalNP_B = L"UD デジタル 教科書体 NP-B";
	static const std::wstring udDegitalNP_R = L"UD デジタル 教科書体 NP-R";
	static const std::wstring onionScript = L"玉ねぎ楷書激無料版v7改";

	static const std::wstring empty = L"";

	switch (font) {
	case Font::Meiryo:
		return meiryo;
		break;
	case Font::YuGothic:
		return yugothic;
		break;
	case Font::YuMincho:
		return yumincho;
	case Font::UDDegitalN_B:
		return udDegitalN_B;
	case Font::UDDegitalN_R:
		return udDegitalN_R;
	case Font::UDDegitalNK_B:
		return udDegitalNK_B;
	case Font::UDDegitalNK_R:
		return udDegitalNK_R;
	case Font::UDDegitalNP_B:
		return udDegitalNP_B;
	case Font::UDDegitalNP_R:
		return udDegitalNP_R;
	case Font::OnionScript:
		return onionScript;
	default:
		return empty;
		break;
	}
}

void TextWrite::WriteOnManager() {
	//描画処理
	TextWriteManager::GetInstance()->WriteText(name_);
}
