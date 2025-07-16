#include "GamePlayState.h"
#include "GamePlayScene.h"

void StartState::Update(GamePlayScene* scene)
{
	scene->UpdateIntervalNum();
	scene->StartInterVal();
	scene->ObjectsUpdate();
	if (scene->IsStartConditionMet())
	{
		scene->ObjectsMoveable();
		scene->ChangeState(std::make_unique<PlayingState>());
	}
}

void PlayingState::Update(GamePlayScene* scene)
{
	scene->ObjectsUpdate();
	scene->TackleCharge();
	scene->playerSpawnRotation();
	if (scene->IsGameEnd())
	{
		scene->GameEndProcess();
		scene->ChangeState(std::make_unique<EndState>());
	}

}

void EndState::Update(GamePlayScene* scene)
{
	scene->ObjectsUpdate();
	scene->TackleCharge();
	if (scene->IsGameClear())
	{
		scene->GameClearProcess();
	}
	else if (scene->IsGameOver())
	{
		scene->GameOverProcess();
	}
}
