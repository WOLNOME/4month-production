#include "FallingObject.h"
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
		object.first.UpdateMatrix();
	}
}

void FallingObject::Draw(const BaseCamera& _camera) {
	//全てのオブジェクトの描画
	for (auto& object : objects_) {
		object.second.first->Draw(object.first, _camera);
	}
}

void FallingObject::CreateObject() {
	//1秒間に2個の確率で生成
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	int lotteryNum = 60;				//抽選回数/1f
	int winningRate = 2 * kDeltaTime;	//当選確率/1f
	int winningNum = 0;					//当選個数
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
		Object object;
		//ワールドトランスフォームの初期化
		WorldTransform objectTransform;
		objectTransform.Initialize();
		objectTransform.translate_ = { 0.0f, 10.0f, 0.0f };
		objectTransform.scale_ = { 1.0f, 1.0f, 1.0f };
		objectTransform.rotate_ = { 0.0f, 0.0f, 0.0f };
		//モデルの生成
		const char* objectName[4] = { "player", "enemy", "Fan", "Freeze" };
		std::uniform_int_distribution<int> rand(0, 3);
		int randomNum = rand(engine);
		std::unique_ptr<Object3d> object = std::make_unique<Object3d>();
		object->InitializeModel(objectName[randomNum]);
		//パーティクルの生成
		const char* particleName;
		if (randomNum == 0) particleName = "deadPlayer";
		else particleName = "deadEnemy";
		std::unique_ptr<Particle> particle = std::make_unique<Particle>();
		particle->Initialize("fallObject" + RandomStringUtil::GenerateRandomString(3), particleName);
		//速度の設定
		Vector3 velocity = { 0.0f, 0.0f, 0.0f };
		//パーティクルのタイマーの設定
		float particleTimer = 0.0f;
		//登録
		

	}
}

void FallingObject::FallObject() {
	//全てのオブジェクトの更新
	for (auto& object : objects_) {
		//オブジェクトの移動
		object.first.translate_.y -= 0.1f;
		//パーティクルの移動
		object.second.second->Update();
	}
	//画面外に出たオブジェクトを削除
	for (auto it = objects_.begin(); it != objects_.end();) {
		if (it->first.translate_.y < -10.0f) {
			it = objects_.erase(it);
		}
		else {
			it++;
		}
	}
}
