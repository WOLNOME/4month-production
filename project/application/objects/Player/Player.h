#pragma once

#include "Input.h"

#include <Framework.h>
#include <vector>
#include <memory>
#include <MyMath.h>
#include <Object3d.h>
#include <Particle.h>

#include "../../../engine/appCollider/AppCollider.h"
#include "../GameObject/GameObject.h"

class Audio;

class Player : public GameObject
{
public:

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw(BaseCamera _camera) override;
	
	// 移動
	void Move();
	
	void MovePosition();

	// 場外処理
	void OutOfField();
	
	// 攻撃
	void Attack();

	// ImGui
	void ImGuiDraw();

	// モデルの更新
	void UpdateModel();

private: // 衝突判定

	// 当たっている間ずっと呼ばれる
	void OnCollision(const AppCollider* _other);

	// 当たった瞬間だけ呼ばれる
	void OnCollisionTrigger(const AppCollider* _other);

	// 障害物にぶつかったとき、最小の移動量で押し戻すベクトルを求める
	Vector3 ComputePenetration(const AppAABB& otherAABB);

public: // ゲッター
	
	// 死亡フラグを取得
	bool IsDead() const { return isDead_; }

	// プレイヤーの速度を取得
	Vector3 GetVelocity() const { return moveVel_; }

	// 落下中かどうか
	bool IsGround() const { return isGround_; }

	// 攻撃チャージフラグを取得
	bool IsChargeMax() const { return *isChargeMax_; }

public: //セッター

	// プレイヤーの位置をセット
	void SetPlayerPos(const Vector3& _pos) { wtPlayer_.translate_ = _pos; }

	// 攻撃チャージフラグをセット
	void SetIsChargeMax(bool* _flag) { isChargeMax_ = _flag; }

	// 落下フラグをセット(クリアシーンで常にONにしてたい)
	void SetIsGround(bool _flag) { isGround_ = _flag; }

	// 動かせるかフラグをセット(クリアシーンで動かないようにさせたい)falseで動かない
	void IsMoveable(bool _flag) { isMoveable_ = _flag; }

private:

	//氷の上の移動
	void MoveOnIce();

	//氷の上の攻撃
	void AttackOnIce();

	//氷の上の移動更新
	void MovePositionOnIce();

	//移動関数の共通部
	void MoveCommon(float moveSpeed);

	//攻撃関数の共通部
	void AttackCommon(float attackSpeed);

	//移動更新関数の共通部
	void MovePositionCommon(float friction);

private:

	// 入力
	Input* input_ = nullptr;

	//効果音のポインタ
	std::unique_ptr<Audio> obstacleCollisionSE_ = nullptr;

	// プレーヤーモデル情報
	WorldTransform wtPlayer_{};
	std::unique_ptr<Object3d> player_ = nullptr;
	uint32_t textureHandle_ = 0u;

	// 当たり判定関係
	AppColliderManager* appColliderManager_ = nullptr;
	std::unique_ptr<AppCollider> appCollider_ = nullptr;
	AppAABB aabb_{};
	bool isGround_ = false;
	bool isStop_ = false;
	// エネミーの位置
	Vector3 enemyPosition_{};
	// ノックバックの時間
	float knockBackTime_ = 0.0f;
	// ノックバックの余韻
	bool isAftertaste_ = false;

	//パーティクル
	std::unique_ptr<Particle> hitEffect_ = nullptr;
	int countHitEffect_;
	std::unique_ptr<Particle> deadEffect_ = nullptr;
	int countDeadEffect_;
	std::unique_ptr<Particle> walkEffect_ = nullptr;
	std::unique_ptr<Particle> tackleEffect_ = nullptr;

	//効果音
	std::unique_ptr<Audio> fallSE = nullptr;
	std::unique_ptr<Audio> cantMoveSE = nullptr;
	std::unique_ptr<Audio> attackSE = nullptr;

	// 移動速度
	float moveSpeed_ = 0.103f;
	Vector3 moveVel_ = { 0.01f,0.01f,0.01f };
	// 落下速度
	float fallSpeed_ = 0.3f;
	// 摩擦係数（減速率）
	float attackFriction_ = 2.0f;
	float friction_ = 29.0f / 30.0f;

	// 攻撃時間
	const float attackTime_ = 12;
	float attackTimeCounter_ = attackTime_;
	// 攻撃チャージがMaxかどうか
	bool* isChargeMax_;

	Vector3 attackToEnemy_{};

	// 死亡フラグ
	bool isDead_ = false;
	//速度低下率、1.0fで通常速度、これを下げていくと速度が遅くなる
	float slowRate_ = 1.0f;

	//氷の上にいるかどうか
	bool onIce_ = false;

	//氷の上にいるときの摩擦係数
	float frictionOnIce_ = 0.98f;
	//氷の上にいるときの移動速度
	float moveSpeedOnIce_ = 0.01f;
	//氷の上にいるときの最高速度
	float MaxSpeedOnIce_ = 0.2f;

	// 動かせるかフラグ(クリアシーンで動かないようにさせたい)
	bool isMoveable_ = true;
	// コックバック時の点滅フラグ
	bool isFlash_ = false;
};

