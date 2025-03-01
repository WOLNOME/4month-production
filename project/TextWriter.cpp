#include "TextWriter.h"

void TextWriter::Initialize() {
	//IDWriteFactoryの生成
	ComPtr<IDWriteFactory> directWriteFactory;
	if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &directWriteFactory)))[[unlikely]] {
		return;
	}
	//ID3D11On12Deviceの生成
https://deep-verdure.hatenablog.com/entry/2022/04/11/025510
}

void TextWriter::Update() {
}

void TextWriter::Draw() {
}
