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
	struct ObjectParam {
		WorldTransform worldTransform;
		std::unique_ptr<Object3d> object3d;
		std::unique_ptr<Particle> deadParticle;
		std::unique_ptr<Particle> sprayParticle;
		Vector3 velocity;
		float particleTimer;
	};

public:
	void Initialize();
	void Finalize();
	void Update();
	void Draw(const BaseCamera& _camera);

	void DebugWithImGui();

private:
	//生成アルゴリズム
	void CreateObject();
	//落下のアルゴリズム
	void FallObject();

private:
	//落下させるオブジェクトのコンテナ
	std::list<ObjectParam> objects_;

	float kGravity = -9.8f;	//重力加速度
	float particleHeight = -75.0f;	//パーティクルの発生する高さ


};

