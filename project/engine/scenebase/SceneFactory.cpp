#include "SceneFactory.h"
#include "DevelopScene.h"
#include "TitleScene.h"
#include "ClearScene.h"

// 追加
#include "StageSelectScene.h"
#include "GamePlayScene.h"
#include "GameOverScene.h"
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
	else if (sceneName == "STAGESELECT"){
		newScene = new StageSelectScene();
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
