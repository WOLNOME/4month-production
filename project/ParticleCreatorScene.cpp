#include "ParticleCreatorScene.h"
#include "ImGuiManager.h"
#include "JsonUtil.h"

void ParticleCreatorScene::Initialize() {
	//シーン共通の初期化
	BaseScene::Initialize();
	json jsonData;
	jsonData["name"] = "particle";
	jsonData["path"] = "Resources/particles/particle";

	JsonUtil::CreateJson("particle.json", "Resources/particles", jsonData);

}

void ParticleCreatorScene::Finalize() {
}

void ParticleCreatorScene::Update() {
	//シーン共通の更新
	BaseScene::Update();

}

void ParticleCreatorScene::Draw() {
}

void ParticleCreatorScene::TextDraw() {
}
