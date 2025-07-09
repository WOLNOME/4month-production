#pragma once
#include "BaseEnemy.h"
#include <random>
#include <Particle.h>

#include "../appCollider/AppCollider.h"
#include "../objects/GameObject/GameObject.h"

class TackleEnemy : public BaseEnemy
{
public:
    void EnemyInitialize(const std::string& filePath) override;
    void EnemyUpdate() override;
    void EnemyDraw(const BaseCamera& camera) override;
    void StartTackle();

	/////////// GameObjectとの競合を無くすための関数 ///////////

    void Initialize() override {}
    void Update() override {}
    void Draw(BaseCamera _camera) override {_camera; }

	///////////////////////////////////////////////////////////


    // 場外処理
    void OutOfField();

private: // 衝突判定

	// 当たっている間ずっと呼ばれる
    void OnCollision(const AppCollider* _other);

	// 当たった瞬間だけ呼ばれる
	void OnCollisionTrigger(const AppCollider* _other);

    // 障害物にぶつかったとき、最小の移動量で押し戻すベクトルを求める
    Vector3 ComputePenetration(const AppAABB& otherAABB);

private:

    /**
	 * \brief
	 *
	 */
	void Move();

    //氷の上の移動
    void MoveOnIce();

private:
	// タックル中かどうか
    bool isTackling_ = false;
    // 初期速度
    float tackleSpeed_ = 20.0f;
    // 摩擦係数（減速率）
    float tackleFriction_ = 2.0f;
    // 速度ベクトル
    Vector3 tackleVelocity_ = { 0.0f, 0.0f, 0.0f };
    // タックルの方向
    Vector3 tackleDirection_ = { 0.0f, 0.0f, 0.0f };
    // タックル待機タイマー
    float tackleWaitTimer_ = 0.0f;
    // 次のタックルまでの待機時間
    float nextTackleWaitTime_ = 0.0f;
    // ランダムエンジン
    std::mt19937 randomEngine_;

    // 落下速度
    float fallSpeed_ = 0.3f;

    bool isHit_ = false;
	bool isStop_ = false;
    // ノックバックの余韻
	bool isAftertaste_ = false;
	
	// ノックバックの時間
	float knockBackTime_ = 0.0f;

    //氷の上にいるときの摩擦係数
    float frictionOnIce_ = 0.995f;

	bool isGrroundPre_ = false;
};