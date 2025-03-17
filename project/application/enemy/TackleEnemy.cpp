#include "TackleEnemy.h"

#include <chrono>

#include "../appCollider/AppCollisionManager.h"

void TackleEnemy::EnemyInitialize(const std::string& filePath)
{
    //オブジェクトの初期化
    object3d_ = std::make_unique<Object3d>();
	object3d_->InitializeModel(filePath);

	//トランスフォームの初期化
    transform_.Initialize();
	transform_.scale_ = { 1.0f, 1.0f, 1.0f };
	transform_.translate_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };

    // ランダムエンジンの初期化（シードを現在時刻から取得）
    randomEngine_.seed(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));

    // 次のタックルまでの待機時間を設定
    std::uniform_real_distribution<float> waitTimeDist(1.0f, 3.0f);
    nextTackleWaitTime_ = waitTimeDist(randomEngine_);
    tackleWaitTimer_ = 0.0f;

    // 当たり判定
    appCollisionManager_ = AppCollisionManager::GetInstance();
    objectName_ = "TackleEnemy";
    appCollider_ = std::make_unique<AppCollider>();
    appCollider_->SetOwner(this);
    appCollider_->SetColliderID(objectName_);
    appCollider_->SetShapeData(&aabb_);
    appCollider_->SetShape(AppShape::AppAABB);
    appCollider_->SetAttribute(appCollisionManager_->GetNewAttribute(appCollider_->GetColliderID()));
    appCollider_->SetOnCollisionTrigger(std::bind(&TackleEnemy::OnCollisionTrigger, this, std::placeholders::_1));
    appCollider_->SetOnCollision(std::bind(&TackleEnemy::OnCollision, this, std::placeholders::_1));
    appCollisionManager_->RegisterCollider(appCollider_.get());
}

void TackleEnemy::EnemyUpdate()
{
	// ノックバック中は移動、攻撃できない
	if (knockBackTime_ > 0.0f)
	{
        isHit_ = false;
		knockBackTime_ -= 1.0f;
    } 
    else
	{

         // タックル中でない場合、待機タイマーを更新
        if (!isTackling_ && isGround_ && !isAftertaste_)
        {
            tackleWaitTimer_ += 1.0f / 60.0f;
            if (tackleWaitTimer_ >= nextTackleWaitTime_)
            {
                // タックルを開始
                StartTackle();

                // 待機タイマーをリセット
                tackleWaitTimer_ = 0.0f;
            }
        }
	}

    //移動
    Move();
    
   

	// 場外処理
	OutOfField();

    //生存中か
	if (transform_.translate_.y <= -10.0f)
	{
		isAlive_ = false;
	}

    //行列の更新
	transform_.UpdateMatrix();

    // 当たり判定
    aabb_.min = transform_.translate_ - transform_.scale_;
    aabb_.max = transform_.translate_ + transform_.scale_;
    appCollider_->SetPosition(transform_.translate_);

}

void TackleEnemy::EnemyDraw(const BaseCamera& camera)
{
    object3d_->Draw(transform_,camera);
}

void TackleEnemy::StartTackle()
{
	isAttack_ = true;

    // ターゲット方向を計算
    tackleDirection_ = target_ - transform_.translate_;
    // Y軸方向の移動はしない
    tackleDirection_.y = 0.0f;
    // 正規化
	tackleDirection_ = tackleDirection_.Normalized();

    // 初期速度を設定
    tackleVelocity_ = tackleDirection_ * tackleSpeed_;

    // 次のタックルまでの待機時間を設定
    std::uniform_real_distribution<float> waitTimeDist(1.0f, 5.0f); // 1秒〜5秒の間でランダム
    nextTackleWaitTime_ = waitTimeDist(randomEngine_);
}

void TackleEnemy::Finalize()
{
    // 各解放処理
    if (appCollider_)
    {
        appCollisionManager_->DeleteCollider(appCollider_.get());
        appCollider_.reset();
    }

}

void TackleEnemy::OutOfField()
{
    if (isGround_ == false)
    {
        transform_.translate_.y -= fallSpeed_;
    }

    isGround_ = false;

    if (transform_.translate_.y < -30.0f)
    {
        isGround_ = true;
    }
}

void TackleEnemy::OnCollision(const AppCollider* _other)
{
    if (_other->GetColliderID() == "Field")
    {
        isGround_ = true;
    }


    if (_other->GetColliderID() == "Player")
    {
        // どちらも攻撃してない状態かつノックバック中でないときプレイヤーに当たった場合
        if (!_other->GetOwner()->IsAttack() && !isAttack_ && !isAftertaste_)
        {
            //////-------押し出される処理-------//////
            // プレイヤーの速度
            Vector3 playerVelocity = _other->GetOwner()->GetVelocity();

            // プレイヤーの進行方向に対して垂直な方向を計算
            Vector3 perpendicularDirection = Vector3(-playerVelocity.z, 0.0f, playerVelocity.x).Normalized();

            // プレイヤーの進行方向に対して垂直な方向にエネミーを移動
            float distance = 0.2f; // プレイヤーからエネミーを徐々に離す距離
            transform_.translate_ += perpendicularDirection * distance;

            // tackleVelocity_ をプレイヤーの速度に設定
            tackleVelocity_ = playerVelocity;
        }

        // 余韻中にプレイヤーに当たった場合
        if (!_other->GetOwner()->IsAttack() && !isAttack_  && isAftertaste_)
        {
            // プレイヤーの位置を取得
            Vector3 playerPosition = _other->GetOwner()->GetPosition();

            // エネミーの位置を取得
            Vector3 enemyPosition = transform_.translate_;

            // プレイヤーからエネミーへの方向ベクトルを計算
            Vector3 direction = enemyPosition - playerPosition;
            direction.Normalize();
			direction.y = 0.0f;

            // エネミーの速度ベクトルを反転させる
            tackleVelocity_ = direction * tackleVelocity_.Length();

        }
    }

	

	// エネミー同士の衝突
	if (_other->GetColliderID() == "TackleEnemy")
	{
        // エネミー同士の衝突処理
        Vector3 enemyPosition = transform_.translate_;
        Vector3 otherEnemyPosition = _other->GetOwner()->GetPosition();

        // エネミー同士が重ならないようにする
        Vector3 direction = enemyPosition - otherEnemyPosition;
        direction.Normalize();
        float distance = 2.5f; // エネミー同士の間の距離を調整するための値

        // 互いに重ならないように少しずつ位置を調整
        if ((enemyPosition - otherEnemyPosition).Length() < distance)
        {
            enemyPosition += direction * 0.1f; // 微調整のための値
            enemyPosition.y = 0.7f;
            transform_.translate_ = enemyPosition;
            position_ = transform_.translate_;
        }
	}
}

void TackleEnemy::OnCollisionTrigger(const AppCollider* _other)
{
    if (_other->GetColliderID() == "Player")
    {
        if (isAttack_)
        {
            isStop_ = true;
        }

        if (_other->GetOwner()->IsAttack()) 
        {
			isHit_ = true;
			isAftertaste_ = true;

            // プレイヤーの位置
            Vector3 playerPosition = _other->GetOwner()->GetPosition();

            // プレイヤーの位置から逃げる
            Vector3 runDirection = transform_.translate_ - playerPosition;

            // ノックバック
            tackleVelocity_ = runDirection;
            tackleVelocity_ *= 9.0f;
            tackleVelocity_.y = 0.0f;
            // ノックバックタイマー
            knockBackTime_ = 4.0f;
        }
    }
}

void TackleEnemy::Move()
{
    // フレーム間の時間差（秒）
    float deltaTime = 1.0f / 60.0f;

    // 摩擦による減速を適用
    Vector3 friction = -tackleVelocity_ * tackleFriction_ * deltaTime;
    tackleVelocity_ += friction;

    // 速度が非常に小さくなったら停止する
    if (tackleVelocity_.Length() < 0.01f or isStop_ == true)
    {
        isAttack_ = false;
        isStop_ = false;

		isAftertaste_ = false;

        return;
    }

    // 位置を更新
    if (!isStop_)
    {
        transform_.translate_ += tackleVelocity_ * deltaTime;
    }
}

