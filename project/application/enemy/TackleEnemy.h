#pragma once
#include "BaseEnemy.h"
#include <random> // ランダム生成のために追加

class TackleEnemy : public BaseEnemy
{
public:
    void Initialize(const std::string& filePath) override;
    void Update() override;
    void Draw(const BaseCamera& camera) override;
    void StartTackle();
    Vector3 GetTargetPosition() const { return target_; }
    void SetTargetPosition(const Vector3& target) { target_ = target; }
    Vector3 GetPosition() const { return transform_.translate_; }
    void SetPosition(const Vector3& position) { transform_.translate_ = position; }

private:
	void UpdateTackle();
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
    // 目標位置
    Vector3 target_ = { 0.0f, 0.0f, 3.0f };
    // タックル待機タイマー
    float tackleWaitTimer_ = 0.0f;
    // 次のタックルまでの待機時間
    float nextTackleWaitTime_ = 0.0f;
    // ランダムエンジン
    std::mt19937 randomEngine_;
};