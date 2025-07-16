#pragma once
#include <memory>
#include <string>

#include "Object3d.h"
#include "Object3dCommon.h"
#include "Audio.h"
#include "Particle.h"
#include "application/appCollider/AppCollisionManager.h"
#include "application/appCollider/AppShape.h"
#include "application/objects/GameObject/GameObject.h"

class EnemyManager;
class BaseCamera;

class BaseEnemy : public GameObject
{
public:
	virtual ~BaseEnemy() = default;
	virtual void EnemyInitialize(const std::string& filePath);
	virtual void EnemyUpdate() = 0;
	virtual void EnemyDraw(const BaseCamera& camera) = 0;
	void Finalize() override;
	void UpdateTransform() { transform_.UpdateMatrix(); }
	bool IsPlayingDeadEffect() const { return deadEffect_->emitter_.isPlay; }
	bool isDeadSEPlayed() const { return isDeadSEPlayed_; }
	void SetDeadSEPlayed(bool played) { isDeadSEPlayed_ = played; }
	void SetMoveable(bool moveable) { isMoveable_ = moveable; }
	bool IsGround() const { return isGround_; }
	bool IsAlive() const { return isAlive_; }

	// アクセッサ
	// Transform情報
	void SetPosition(const Vector3& position) { transform_.translate_ = position; }
	void SetRotate(const Vector3& rotate) { transform_.rotate_ = rotate; }
	void SetScale(const Vector3& scale) { transform_.scale_ = scale; }
	Vector3 GetPosition() const { return transform_.translate_; }
	Vector3 GetRotate() const { return transform_.rotate_; }
	Vector3 GetScale() const { return transform_.scale_; }

	// ターゲット
	void SetTargetPosition(const Vector3& target) { targetPosition_ = target; }

	// マネージャー
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }

	// タイプ
	std::string GetType() const { return type; }

protected:
	// タイプ
	std::string type = "BaseEnemy";
	//3Dオブジェクト
	std::unique_ptr<Object3d> object3d_;
	//トランスフォーム
	WorldTransform transform_ = {};
	// ターゲット
	Vector3 targetPosition_ = { 0.0f,1.0f,3.0f };		// 適当な数値を入れておく
	//氷の上にいるかどうか
	bool onIce_ = false;
	// SE
	std::unique_ptr<Audio> fallSE_ = nullptr;
	//パーティクル
	std::unique_ptr<Particle> deadEffect_ = nullptr;
	int countDeadEffect_ = 0;
	// 死亡時の効果音再生フラグ
	bool isDeadSEPlayed_ = false;

	//エネミーマネージャーのポインタ
	EnemyManager* enemyManager_ = nullptr;
	//当たり判定
	AppCollisionManager* appCollisionManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_ = nullptr;
	AppAABB aabb_;
	// 行動不能フラグ
	bool isMoveable_ = true;
	//地面にいるか
	bool isGround_ = true;
	//生存フラグ
	bool isAlive_ = true;
};

