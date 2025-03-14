#include "SceneFactory.h"
#include "DevelopScene.h"
#include "TitleScene.h"
#include "ClearScene.h"

// 追加
#include "GamePlayScene.h"
// パーティクルクリエイター
#include "ParticleCreatorScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	//次のシーンを生成
	BaseScene* newScene = nullptr;

	if (sceneName == "DEVELOP") {
		newScene = new DevelopScene();
	}
	else if (sceneName == "TITLE") {
		newScene = new TitleScene();
	}
	else if (sceneName == "CLEAR") {
		newScene = new ClearScene();
	}
	// 追加
	else if (sceneName == "GAMEPLAY") {
		newScene = new GamePlayScene();
	}
	//パーティクルクリエイター
	else if (sceneName == "PARTICLECREATOR") {
		newScene=new ParticleCreatorScene():
	}

	return newScene;
}
