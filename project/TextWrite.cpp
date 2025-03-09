#include "TextWrite.h"
#include "TextWriteManager.h"

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

void TextWrite::SetParam(const Vector2& position, float width, float height, const Font& font, float size, const D2D1::ColorF& color) {
	position_ = position;
	width_ = width;
	height_ = height;
	font_ = font;
	fontName_ = ReturnFontName(font_);
	size_ = size;
	color_ = color;
}

const std::wstring& TextWrite::ReturnFontName(const Font& font) {
	std::wstring fontName;
	switch (font) {
	case Font::Meiryo:
		fontName = L"メイリオ";
		break;
	case Font::YuGothic:
		fontName = L"游ゴシック";
		break;
	default:
		break;
	}
	return fontName;
}
