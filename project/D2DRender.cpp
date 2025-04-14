#include "D2DRender.h"

D2DRender* D2DRender::instance = nullptr;

D2DRender* D2DRender::GetInstance() {
	if (instance == nullptr) {
		instance = new D2DRender;
	}
	return instance;
}

void D2DRender::Initialize() {
}

void D2DRender::Update() {
}

void D2DRender::Finalize() {
	delete instance;
	instance = nullptr;
}

void D2DRender::Draw() {
}
