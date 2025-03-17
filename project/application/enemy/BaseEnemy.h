#pragma once
#include <memory>
#include <string>

#include "Object3d.h"
#include "Object3dCommon.h"

class BaseCamera;

class BaseEnemy
{
public:
	virtual ~BaseEnemy() = default;
	virtual void EnemyInitialize(const std::string& filePath) = 0;
	virtual void EnemyUpdate() = 0;
	virtual void EnemyDraw(const BaseCamera& camera) = 0;
protected:
	//3Dオブジェクト
	std::unique_ptr<Object3d> object3d_;
	//トランスフォーム
	WorldTransform transform_;

	//氷の上にいるかどうか
	bool onIce_ = false;
};

