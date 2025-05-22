#include "Player.h"

#include "../../appCollider/AppCollisionManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "ParticleManager.h"
#include "TextureManager.h"

void Player::Initialize() {
	input_ = Input::GetInstance();

	// プレーヤーモデル
	wtPlayer_.Initialize();
	wtPlayer_.scale_ = scale_;
	wtPlayer_.translate_ = position_;
	player_ = std::make_unique<Object3d>();
	player_->InitializeModel("player");

	textureHandle_ = TextureManager::GetInstance()->LoadTexture("player.png");

	// 当たり判定関係
	appCollisionManager_ = AppCollisionManager::GetInstance();

	objectName_ = "Player";
	appCollider_ = std::make_unique<AppCollider>();
	appCollider_->SetOwner(this);
	appCollider_->SetColliderID(objectName_);
	appCollider_->SetShapeData(&aabb_);
	appCollider_->SetShape(AppShape::AppAABB);
	appCollider_->SetAttribute(appCollisionManager_->GetNewAttribute(appCollider_->GetColliderID()));
	appCollider_->SetOnCollisionTrigger(std::bind(&Player::OnCollisionTrigger, this, std::placeholders::_1));
	appCollider_->SetOnCollision(std::bind(&Player::OnCollision, this, std::placeholders::_1));
	appCollisionManager_->RegisterCollider(appCollider_.get());

	//パーティクル
	auto particleManager = ParticleManager::GetInstance();

	hitEffect_ = std::make_unique<Particle>();
	hitEffect_->Initialize(particleManager->GenerateName("star"), "star");
	hitEffect_->emitter_.isGravity = true;
	hitEffect_->emitter_.gravity = -150.0f;
	hitEffect_->emitter_.isBound = true;
	hitEffect_->emitter_.repulsion = 0.8f;
	hitEffect_->emitter_.floorHeight = 0.0f;
	hitEffect_->emitter_.isPlay = false;
	countHitEffect_ = 0;

	deadEffect_ = std::make_unique<Particle>();
	deadEffect_->Initialize(particleManager->GenerateName("deadPlayer"), "deadPlayer");
	deadEffect_->emitter_.isGravity = true;
	deadEffect_->emitter_.gravity = -150.0f;
	deadEffect_->emitter_.isPlay = false;
	countDeadEffect_ = 0;

	walkEffect_ = std::make_unique<Particle>();
	walkEffect_->Initialize(particleManager->GenerateName("walkPlayer"), "walkPlayer");

	tackleEffect_ = std::make_unique<Particle>();
	tackleEffect_->Initialize(particleManager->GenerateName("tackleEnemy"), "tackle");
	tackleEffect_->emitter_.isGravity = true;
	tackleEffect_->emitter_.gravity = 0.3f;
	tackleEffect_->emitter_.isPlay = false;

	fallSE = std::make_unique<Audio>();
	fallSE->Initialize("soundeffect/fall.wav");
}

void Player::Finalize() {
	// 各解放処理
	if (appCollider_) {
		appCollisionManager_->DeleteCollider(appCollider_.get());
		appCollider_.reset();
	}

	player_.reset();
}

void Player::Update() {

	walkEffect_->emitter_.isPlay = false;
	// ノックバック中は移動、攻撃できない
	if (knockBackTime_ > 0.0f) {
		knockBackTime_ -= 1.0f;
	}
	else if (!isAftertaste_ && isMoveable_) {
		//氷の上にいるとき
		if (onIce_) {
			//移動
			MoveOnIce();

			// 攻撃	
			AttackOnIce();
		}
		else {
			// 移動
			Move();

			// 攻撃	
			Attack();
		}
	}

	//氷の上にいるとき
	if (onIce_) {
		MovePositionOnIce();
	}
	else {
		// 位置更新
		MovePosition();
	}

	if (isAftertaste_) {
		// 点滅処理
		static float time = 0.0f;
		static const float flashInterval = 15.0f; // 点滅間隔（フレーム数）

		time += 1.0f;

		if (time >= flashInterval) {
			isFlash_ = !isFlash_;  // ON/OFFを切り替える
			time = 0.0f;
		}
	}

	// 場外処理
	OutOfField();

	//パーティクル処理
	if (hitEffect_->emitter_.isPlay) {
		countHitEffect_++;
		if (countHitEffect_ > 30) {
			hitEffect_->emitter_.isPlay = false;
			countHitEffect_ = 0;
		}
	}
	if (deadEffect_->emitter_.isPlay || countDeadEffect_ != 0) {
		countDeadEffect_++;
		if (countDeadEffect_ == 30) {
			deadEffect_->emitter_.isPlay = false;
		}
		else if (countDeadEffect_ > 60) {
			isDead_ = true;
			countDeadEffect_ = 0;
		}
	}

	wtPlayer_.UpdateMatrix();
	wtPlayer_.rotate_ = rotation_;
	wtPlayer_.scale_ = scale_;

	// 当たり判定関係
	aabb_.min = wtPlayer_.translate_ - wtPlayer_.scale_;
	aabb_.max = wtPlayer_.translate_ + wtPlayer_.scale_;
	appCollider_->SetPosition(wtPlayer_.translate_);

	onIce_ = false;
}

void Player::Draw(BaseCamera _camera) {
	if (!isDead_ && !deadEffect_->emitter_.isPlay && !isFlash_) {
		player_->Draw(wtPlayer_, _camera,nullptr,textureHandle_);
	}
}

void Player::Move() {

	moveVel_ = {0.0f, 0.0f, 0.0f};

	MoveCommon(moveSpeed_);

}

void Player::MovePosition() {

	MovePositionCommon(friction_);
}

void Player::OutOfField() {
	if (isGround_ == false) {
		wtPlayer_.translate_.y -= fallSpeed_;
	}
	else {
		wtPlayer_.translate_.y = 1.0f;
	}

	if (wtPlayer_.translate_.y < -10.0f && countDeadEffect_ == 0) {
		deadEffect_->emitter_.isPlay = true;
		deadEffect_->emitter_.transform.translate = wtPlayer_.translate_;
		isGround_ = true;
		isDeadShake_ = true;
		fallSE->Play(false, 0.5f);
	}

	isGround_ = false;
}

void Player::Attack() {

	AttackCommon(2.0f);

}

void Player::ImGuiDraw() {
	// プレイヤー
	ImGui::Begin("Player");

	ImGui::SliderFloat3("PlayerPos", &wtPlayer_.translate_.x, -20.0f, 20.0f);

	ImGui::SliderFloat3("PlayerSpeed", &moveVel_.x, -5.0f, 5.0f);

	ImGui::Text("isGround_ : %s", isGround_ ? "true" : "false");

	if (ImGui::Button("ReSetPos")) {
		wtPlayer_.translate_ = { 0.0f,3.0f,0.0f };
	}

	ImGui::Text("aabb.max: %.3f %.3f %.3f", aabb_.max.x, aabb_.max.y, aabb_.max.z);
	ImGui::Text("aabb.min: %.3f %.3f %.3f", aabb_.min.x, aabb_.min.y, aabb_.min.z);

	ImGui::End();

}

void Player::UpdateModel()
{
	wtPlayer_.scale_ = scale_;
	wtPlayer_.rotate_ = rotation_;
	wtPlayer_.translate_ = position_;

	wtPlayer_.UpdateMatrix();
}

void Player::OnCollision(const AppCollider* _other) {
	// フィールドの内にいるかどうか
	if (_other->GetColliderID() == "Field") {
		isGround_ = true;
	}
	else if (_other->GetColliderID() == "Obstacle") {
		wtPlayer_.translate_ += ComputePenetration(*_other->GetAABB());
		wtPlayer_.UpdateMatrix();

		// 当たり判定関係
		aabb_.min = wtPlayer_.translate_ - wtPlayer_.scale_;
		aabb_.max = wtPlayer_.translate_ + wtPlayer_.scale_;
		appCollider_->SetPosition(wtPlayer_.translate_);
	}
	else if (_other->GetColliderID() == "Bumper") {
		Vector3 penetration = ComputePenetration(*_other->GetAABB());
		wtPlayer_.translate_ += penetration;
		penetration.Normalize();
		// ノックバック
		moveVel_ = penetration;
		moveVel_ /= 3.0f;
		moveVel_.y = 0.0f;
		// ノックバックタイマー
		knockBackTime_ = 30.0f;

		isAttack_ = false;
		// 攻撃エフェクトオフ
		tackleEffect_->emitter_.isPlay = false;
	}
	else if (_other->GetColliderID() == "IceFloor") {
		onIce_ = true;
	}

	// どちらも攻撃していなくてノックバック中でないとき
	if (_other->GetColliderID() == "TackleEnemy" && !_other->GetOwner()->IsAttack() && !isAttack_ && !isAftertaste_ && isMoveable_) {
		// プレイヤーの速度を取得
		Vector3 playerVelocity = moveVel_;

		// 減速係数を設定
		float decelerationFactor = 0.5f; // 速度を50%に減少させる

		// プレイヤーの速度を減少させる
		playerVelocity *= decelerationFactor;

		// プレイヤーの速度を更新
		moveVel_ = playerVelocity;

	}

	// プレイヤー同士の衝突
	if (_other->GetColliderID() == "Player" && isMoveable_) {
		// プレイヤー同士の衝突処理
		Vector3 playerPosition = wtPlayer_.translate_;
		Vector3 otherPlayerPosition = _other->GetOwner()->GetPosition();

		// プレイヤー同士が重ならないようにする
		Vector3 direction = playerPosition - otherPlayerPosition;
		direction.Normalize();
		float distance = 2.5f; // プレイヤー同士の間の距離を調整するための値

		// 互いに重ならないように少しずつ位置を調整
		if ((playerPosition - otherPlayerPosition).Length() < distance) {
			playerPosition += direction * 0.1f; // 微調整のための値
			playerPosition.y = 1.0f;
			wtPlayer_.translate_ = playerPosition;
			position_ = wtPlayer_.translate_;
		}
	}

	//アイスミストに当たっている間速度低下
	slowRate_ = 1.0f;
	if (_other->GetColliderID() == "IceMist") {
		slowRate_ = 0.25f;
	}

}

void Player::OnCollisionTrigger(const AppCollider* _other) {
	if (_other->GetColliderID() == "TackleEnemy" && isMoveable_) {
		// 攻撃が当たったとき攻撃を止める
		if (isAttack_) {
			isStop_ = true;
		}

		// エネミーの攻撃を食らったとき
		if (_other->GetOwner()->IsAttack() && !isAttack_) {
			isAftertaste_ = true;
			textureHandle_ = TextureManager::GetInstance()->LoadTexture("player_knockBack.png");

			// 当たったエネミーの位置を取得
			enemyPosition_ = _other->GetOwner()->GetPosition();

			attackToEnemy_ = wtPlayer_.translate_ - enemyPosition_;
			attackToEnemy_.y = 0.0f;
			attackToEnemy_.Normalize();
			attackToEnemy_ *= 1.0f / 3.0f;
			// ノックバック
			moveVel_ = attackToEnemy_;
			// ノックバックタイマー
			knockBackTime_ = 40.0f;

			//パーティクルをオン
			hitEffect_->emitter_.isPlay = true;
			hitEffect_->emitter_.transform.translate = wtPlayer_.translate_;

			//被弾時のシェイクを送る合図
			isDamageShake_ = true;
		}

	}

	// 風に当たったらノックバック
	if (_other->GetColliderID() == "Wind" && !isAttack_ && isMoveable_) {		

		//当たった風の位置を取得
		Vector3 windDirection = wtPlayer_.translate_ - _other->GetOwner()->GetPosition();
		// ノックバック
		moveVel_ += windDirection * 2.0f;
		moveVel_ /= 60.0f;
		moveVel_.y = 0.0f;
		// ノックバックタイマー
		knockBackTime_ = 25.0f;
	}

	// 障害物に当たったら効果音の再生
	if (_other->GetColliderID() == "Obstacle" && isMoveable_)
	{
		if (!obstacleSE_) { return; }
		obstacleSE_->Play();
	}
}

Vector3 Player::ComputePenetration(const AppAABB& otherAABB) {
	Vector3 penetration;

	//X軸方向に押し戻すベクトル
	float overlapX1 = otherAABB.max.x - aabb_.min.x;
	float overlapX2 = aabb_.max.x - otherAABB.min.x;
	float penetrationX = (overlapX1 < overlapX2) ? overlapX1 : -overlapX2;

	//Z軸方向に押し戻すベクトル
	float overlapZ1 = otherAABB.max.z - aabb_.min.z;
	float overlapZ2 = aabb_.max.z - otherAABB.min.z;
	float penetrationZ = (overlapZ1 < overlapZ2) ? overlapZ1 : -overlapZ2;

	//ベクトルの絶対値を求める
	float absX = std::abs(penetrationX);
	float absZ = std::abs(penetrationZ);

	//最小のベクトルを求める
	if (absX < absZ) {
		penetration.x = penetrationX;
	}
	else {
		penetration.z = penetrationZ;
	}

	return penetration;
}

void Player::MoveOnIce() {

	MoveCommon(moveSpeedOnIce_);

	//速度を最高速度以下に抑える
	if (moveVel_.Length() > MaxSpeedOnIce_) {
		moveVel_.Normalize();
		moveVel_ *= MaxSpeedOnIce_;
	}
}

void Player::AttackOnIce()
{
	AttackCommon(1.5f);
}

void Player::MovePositionOnIce() {

	MovePositionCommon(frictionOnIce_);

}

void Player::MoveCommon(float moveSpeed)
{
	// 地面にいるとき
	if (isGround_ && !isStop_) {
		// 移動
		walkEffect_->emitter_.transform.translate = wtPlayer_.translate_;
		if (input_->PushKey(DIK_W)) {
			moveVel_.z += moveSpeed;
			walkEffect_->emitter_.isPlay = true;
		}
		if (input_->PushKey(DIK_S)) {
			moveVel_.z -= moveSpeed;
			walkEffect_->emitter_.isPlay = true;
		}
		if (input_->PushKey(DIK_A)) {
			moveVel_.x -= moveSpeed;
			walkEffect_->emitter_.isPlay = true;
		}
		if (input_->PushKey(DIK_D)) {
			moveVel_.x += moveSpeed;
			walkEffect_->emitter_.isPlay = true;
		}
	}

	if (moveVel_.Length() > 0.0f)
	{
		// 移動方向に向きを変更
		float targetRotationY = atan2f(moveVel_.x, moveVel_.z);
		rotation_.y = MyMath::Lerp(rotation_.y, targetRotationY, 0.1f); // 0.1fは補間係数
	}
}

void Player::AttackCommon(float attackSpeed)
{
	if (input_->TriggerKey(DIK_SPACE) && !isAttack_ && *isChargeMax_)
	{
		isAttack_ = true;

		// 攻撃時間リセット
		attackTimeCounter_ = attackTime_;

	}

	if (isAttack_) {

		// 速度がゼロの場合、プレイヤーの向きに応じて初期速度を設定
		if (moveVel_.x == 0.0f && moveVel_.y == 0.0f && moveVel_.z == 0.0f) {
			// プレイヤーの向きを基に攻撃方向を設定
			moveVel_.x = sinf(rotation_.y) * moveSpeed_; // X方向の速度
			moveVel_.z = cosf(rotation_.y) * moveSpeed_; // Z方向の速度
		}

		moveVel_ *= attackSpeed * attackFriction_;

		attackTimeCounter_ -= 1.0f;

		// 攻撃エフェクト
		tackleEffect_->emitter_.isPlay = true;
		tackleEffect_->emitter_.transform.translate = wtPlayer_.translate_;
		tackleEffect_->emitter_.transform.translate.y += 0.5f;
		tackleEffect_->emitter_.transform.scale = { 0.5f,0.5f,0.5f };
	}

	if (attackTimeCounter_ <= 0.0f) {
		isAttack_ = false;
		isStop_ = false;

		// 攻撃エフェクトオフ
		tackleEffect_->emitter_.isPlay = false;
	}
}

void Player::MovePositionCommon(float friction)
{
	// 摩擦による減速を適用
	moveVel_ *= friction * slowRate_;

	// 速度が非常に小さくなったら停止する
	if (moveVel_.Length() < 0.001f) {
		moveVel_ = { 0.0f, 0.0f, 0.0f };
		return;
	}
	if (moveVel_.Length() < 0.025f) {
		isAftertaste_ = false;
		isFlash_ = false;
		textureHandle_ = TextureManager::GetInstance()->LoadTexture("player.png");
	}

	// 位置を更新
	wtPlayer_.translate_ += moveVel_;
	position_ = wtPlayer_.translate_;

}
