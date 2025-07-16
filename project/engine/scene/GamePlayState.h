#pragma once
class GamePlayScene;

class GamePlayState
{
public:
	virtual ~GamePlayState() = default;
	virtual void Update(GamePlayScene* scene) = 0;
};

class StartState : public GamePlayState
{
public:
	void Update(GamePlayScene* scene) override;
};

class PlayingState : public GamePlayState
{
public:
	void Update(GamePlayScene* scene) override;
};

class EndState : public GamePlayState
{
public:
	void Update(GamePlayScene* scene) override;
};
