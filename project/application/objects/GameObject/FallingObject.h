#pragma once
#include "BaseCamera.h"
#include "WorldTransform.h"
#include "Object3d.h"
#include "Particle.h"
#include "MyMath.h"
#include <list>

class FallingObject {
public:
	void Initialize();
	void Finalize();
	void Update();
	void Draw(const BaseCamera& _camera);

private:
	//生成アルゴリズム
	void CreateObject();
	//落下のアルゴリズム
	void FallObject();

private:
	//落下させるオブジェクトのコンテナ
	std::list<std::pair<WorldTransform, std::pair<std::unique_ptr<Object3d>, std::unique_ptr<Particle>>>> objects_;


};

