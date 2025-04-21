#pragma once
#include "BaseCamera.h"
#include "WorldTransform.h"
#include "Object3d.h"
#include "Particle.h"
#include "MyMath.h"
#include <list>

class FallingObject {
private:
	//オブジェクトの型
	struct Object {
		WorldTransform worldTransform;
		std::unique_ptr<Object3d> object3d;
		std::unique_ptr<Particle> particle;
		Vector3 velocity;
		float particleTimer;
	};

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
	std::list<Object> objects_;

	float particleHeight = 0.0f;	//パーティクルの発生する高さ

};

