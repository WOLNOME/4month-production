#include "FallingObject.h"
#include "ImGuiManager.h"
#include "RandomStringUtil.h"
#include <random>

void FallingObject::Initialize() {
}

void FallingObject::Finalize() {
}

void FallingObject::Update() {
	//生成アルゴリズム
	CreateObject();
	//落下のアルゴリズム
	FallObject();
	//全てのオブジェクトの更新
	for (auto& object : objects_) {
		object.worldTransform.UpdateMatrix();
	}
}

void FallingObject::Draw(const BaseCamera& _camera) {
	//全てのオブジェクトの描画
	for (auto& object : objects_) {
		object.object3d->Draw(object.worldTransform, _camera);
	}
}

void FallingObject::DebugWithImGui() {
#ifdef _DEBUG
	//ImGuiでのデバッグ表示
	ImGui::Begin("落下オブジェクト");
	ImGui::DragFloat("重力加速度", &kGravity, 0.1f);
	ImGui::DragFloat("パーティクルの発生する高さ", &particleHeight, 0.1f);
	ImGui::End();
#endif // _DEBUG
}

void FallingObject::CreateObject() {
	//1秒間に1個の確率で生成
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	int lotteryNum = 60;						//抽選回数/1f
	float winningRate = 1.0f * kDeltaTime;		//当選確率/1f
	int winningNum = 0;							//当選個数
	//当選確率の計算(十万分率)
	float winningProbability = winningRate / lotteryNum * 100000;

	//60回の抽選
	for (int i = 0; i < lotteryNum; i++) {
		//ランダムで0~99999の整数を生成
		std::uniform_int_distribution<int> rand(0, 99999);
		int randomNum = rand(engine);
		//当選確率の範囲内であれば当選
		if (randomNum < winningProbability) {
			//当選個数をインクリメント
			winningNum++;
		}
	}
	//当選個数が0なら抜ける
	if (winningNum == 0) return;
	//当選個数分だけオブジェクトを生成
	for (int i = 0; i < winningNum; i++) {
		//オブジェクト
		ObjectParam objectParam;
		//ワールドトランスフォームの初期化
		objectParam.worldTransform.Initialize();
		std::uniform_real_distribution<float> dist(-23.0f, 23.0f);
		objectParam.worldTransform.translate_ = { dist(engine), 10.0f, dist(engine) + 60.0f };
		objectParam.worldTransform.scale_ = { 1.0f, 1.0f, 1.0f };
		objectParam.worldTransform.rotate_ = { 0.0f, 0.0f, 0.0f };
		//モデルの生成
		const char* objectName[4] = { "player", "enemy", "Fan", "Freeze" };
		std::uniform_int_distribution<int> rand(0, 3);
		int randomNum = rand(engine);
		objectParam.object3d = std::make_unique<Object3d>();
		objectParam.object3d->InitializeModel(objectName[randomNum]);
		//死亡パーティクルの生成
		const char* particleName;
		if (randomNum == 0) particleName = "deadPlayer";
		else particleName = "deadEnemy";
		objectParam.deadParticle = std::make_unique<Particle>();
		objectParam.deadParticle->Initialize("fallObject" + RandomStringUtil::GenerateRandomString(3), particleName);
		objectParam.deadParticle->emitter_.isPlay = false;
		objectParam.deadParticle->emitter_.isGravity = true;
		objectParam.deadParticle->emitter_.gravity = -280.0f;
		//散布パーティクルの生成
		objectParam.sprayParticle = std::make_unique<Particle>();
		objectParam.sprayParticle->Initialize("fallObject" + RandomStringUtil::GenerateRandomString(4), "walkPlayer");
		objectParam.sprayParticle->emitter_.transform.scale = { 0.5f, 0.5f, 0.5f };
		//速度の設定
		objectParam.velocity = { 0.0f, 0.0f, 0.0f };
		//パーティクルのタイマーの設定
		objectParam.particleTimer = 0.0f;
		//登録
		objects_.push_back(std::move(objectParam));
	}
}

void FallingObject::FallObject() {
	//パーティクルが規定時間に到達したら削除
	objects_.remove_if([](const ObjectParam& object) {
		return object.particleTimer > 1.7f;
		});

	//全てのオブジェクトの更新
	for (auto& object : objects_) {
		//緩やかな回転処理
		object.worldTransform.rotate_.x += 1.5f * kDeltaTime;
		object.worldTransform.rotate_.y += 1.8f * kDeltaTime;
		object.worldTransform.rotate_.z += 1.4f * kDeltaTime;
		//重力処理
		object.velocity.y += kGravity * kDeltaTime;
		//速度加算
		object.worldTransform.translate_ += object.velocity * kDeltaTime;
		//死亡パーティクルの発生
		if (object.worldTransform.translate_.y < particleHeight && object.particleTimer == 0.0f) {
			object.deadParticle->emitter_.isPlay = true;
			object.deadParticle->emitter_.transform.translate = object.worldTransform.translate_;
		}
		//パーティクルの更新
		if (object.deadParticle->emitter_.isPlay || object.particleTimer != 0.0f) {
			//死亡パーティクルのカウント
			object.particleTimer += kDeltaTime;
			//規定時間を超えたら生成を止める
			if (object.particleTimer > 0.2f) {
				object.deadParticle->emitter_.isPlay = false;
			}
		}
		if (object.sprayParticle->emitter_.isPlay) {
			//散布パーティクルの座標設定
			object.sprayParticle->emitter_.transform.translate = object.worldTransform.translate_;
		}
	}
}
