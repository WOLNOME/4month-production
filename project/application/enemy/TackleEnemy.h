#pragma once
#include "BaseEnemy.h"
#include <random>

#include "../appCollider/AppCollider.h"
#include "../objects/GameObject/GameObject.h"

class TackleEnemy : public BaseEnemy, public GameObject
{
public:
    void EnemyInitialize(const std::string& filePath) override;
    void EnemyUpdate() override;
    void EnemyDraw(const BaseCamera& camera) override;
    void StartTackle();
    Vector3 GetTargetPosition() const { return target_; }
    void SetTargetPosition(const Vector3& target) { target_ = target; }
    Vector3 GetPosition() const { return transform_.translate_; }
    void SetPosition(const Vector3& position) { transform_.translate_ = position; }

	/////////// GameObjectとの競合を無くすための関数 ///////////

    void Initialize() override {}
    void Update() override {}
    void Draw(BaseCamera _camera) override {_camera; }

	///////////////////////////////////////////////////////////
    

    void Finalize() override;

    // 場外処理
    void OutOfField();
	//生存フラグ
	bool IsAlive() const { return isAlive_; }

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
    //生存フラグ
	bool isAlive_ = true;
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
    // 目標位置
    Vector3 target_ = { 0.0f, 0.0f, 3.0f };
    // タックル待機タイマー
    float tackleWaitTimer_ = 0.0f;
    // 次のタックルまでの待機時間
    float nextTackleWaitTime_ = 0.0f;
    // ランダムエンジン
    std::mt19937 randomEngine_;

    // 落下速度
    float fallSpeed_ = 0.3f;

    // 当たり判定
    AppCollisionManager* appCollisionManager_ = nullptr;
    std::unique_ptr<AppCollider> appCollider_ = nullptr;
    AppAABB aabb_{};
    bool isHit_ = false;
    bool isGround_ = false;

	bool isStop_ = false;
    // ノックバックの余韻
	bool isAftertaste_ = false;
	
	// ノックバックの時間
	float knockBackTime_ = 0.0f;

    //氷の上にいるときの摩擦係数
    float frictionOnIce_ = 0.995f;
};