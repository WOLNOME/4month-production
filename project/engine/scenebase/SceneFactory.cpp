#include "SceneFactory.h"
#include "DevelopScene.h"
#include "application/scene/TitleScene.h"
#include "application/scene/ClearScene.h"
#include "application/scene/StageSelectScene.h"
#include "application/scene/TutorialScene.h"
#include "application/scene/GamePlayScene.h"
#include "application/scene/GameOverScene.h"
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
	else if (sceneName == "STAGESELECT"){
		newScene = new StageSelectScene();
	}
	else if (sceneName == "TUTORIAL") {
		newScene = new TutorialScene();
		TutorialScene::stageNum_ = 0;
	}
	else if (sceneName == "GAMEPLAY") {
		newScene = new GamePlayScene();
		GamePlayScene::stageNum_ = 1;
	}
	else if (sceneName == "GAMEPLAY2") {
		newScene = new GamePlayScene();
		GamePlayScene::stageNum_ = 2;
	}
	else if (sceneName == "GAMEPLAY3") {
		newScene = new GamePlayScene();
		GamePlayScene::stageNum_ = 3;
	}
	else if (sceneName == "GAMEPLAY4") {
		newScene = new GamePlayScene();
		GamePlayScene::stageNum_ = 4;
	} 
	else if (sceneName == "GAMEPLAY5") {
		newScene = new GamePlayScene();
		GamePlayScene::stageNum_ = 5;
	}
	else if (sceneName == "GAMEOVER") {
		newScene = new GameOverScene();
	}
	//パーティクルクリエイター
	else if (sceneName == "PARTICLECREATOR") {
		newScene = new ParticleCreatorScene();
	}

	return newScene;
}
