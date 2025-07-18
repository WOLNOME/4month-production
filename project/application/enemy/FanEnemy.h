#pragma once
#include <random>
#include <Particle.h>

#include "BaseEnemy.h"
#include "../../../engine/appCollider/AppCollider.h"
#include "application/objects/GameObject/GameObject.h"

class EnemyManager;

class FanEnemy : public BaseEnemy, public GameObject
{
public:
	~FanEnemy();
	void EnemyInitialize(const std::string& filePath) override;
	void EnemyUpdate() override;
	void EnemyDraw(const BaseCamera& camera) override;
	void OnCollision(const AppCollider* _other);
	void OnCollisionTrigger(const AppCollider* other);

	void Initialize() override {}
	void Update() override{}
	void Draw(BaseCamera camera) override { camera; }
	void Finalize() override{}

	void SetPosition(const Vector3& position) { transform_.translate_ = position; }
	Vector3 GetPosition() const { return transform_.translate_; }
	void SetRotate(const Vector3& rotate) { transform_.rotate_ = rotate; }
	Vector3 GetRotate() const { return transform_.rotate_; }
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }
	bool IsAlive() const { return isAlive_; }
	bool IsGround() const { return isGround_; }
	void UpdateTransform() { transform_.UpdateMatrix(); }
	void SetMoveable(bool moveable) { isMoveable_ = moveable; }
	bool IsPlayingDeadEffect() const { return deadEffect_->emitter_.isPlay; }
	bool isDeadSEPlayed_ = false;

private:
	void Move();
	void StartFan();
	void FanUpdate();
	void ChageRotationSpeed();
	void OutOfField();
	void UpdateCollider();

	// 障害物にぶつかったとき、最小の移動量で押し戻すベクトルを求める
	Vector3 ComputePenetration(const AppAABB& otherAABB);

	//氷の上の移動
	void MoveOnIce();

private:
	//パーティクル
	std::unique_ptr<Particle> deadEffect_ = nullptr;
	int countDeadEffect_;

	//速度ベクトル
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	//摩擦係数
	const float friction_ = 2.0f;
	//回転速度
	float rotateSpeed_ = 0.02f;
	//回転速度変更インターバル
	float rotateSpeedChangeInterval_ = 3.0f;
	//回転速度変更タイマー
	float rotateSpeedChangeTimer_ = 0.0f;
	//風を生成する間隔
	float windSpawnInterval_ = 0.1f;
	//風を生成するタイマー
	float windSpawnTimer_ = 0.0f;
	//地面にいるか
	bool isGround_ = true;
	//ランダムエンジン
	std::mt19937 randomEngine_;
	//エネミーマネージャーのポインタ
	EnemyManager* enemyManager_ = nullptr;
	//当たり判定
	AppColliderManager* appColliderManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_ = nullptr;
	AppAABB aabb_;
	AppCollider::AppColliderDesc desc = {};
	bool isAlive_ = true;

	//氷の上にいるときの摩擦係数
	float frictionOnIce_ = 0.995f;

	// 行動不能フラグ
	bool isMoveable_ = true;
};

