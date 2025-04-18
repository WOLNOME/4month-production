#include "FallingObject.h"
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
		//ワールドトランスフォームの初期化
		WorldTransform objectTransform;
		objectTransform.Initialize();
		objectTransform.translate_ = { 0.0f, 10.0f, 0.0f };
		objectTransform.scale_ = { 1.0f, 1.0f, 1.0f };
		objectTransform.rotate_ = { 0.0f, 0.0f, 0.0f };
		//モデルの初期化
		std::random_device seed_gen;
		std::mt19937 engine(seed_gen());
		const char* objectName[] = { "player", "enemy", "Fan", "Freeze" };


		std::unique_ptr<Object3d> object = std::make_unique<Object3d>();
		object->InitializeModel("cube");
		//パーティクルの初期化
		std::unique_ptr<Particle> particle = std::make_unique<Particle>();
		particle->Initialize("RainbowSnow", "RainbowSnow");
		particle->emitter_.transform.translate = objectTransform.translate_;
		particle->emitter_.transform.scale = { 1.5f,1.5f,1.5f };
		//登録
		std::pair<std::unique_ptr<Object3d>, std::unique_ptr<Particle>> objectPair(std::move(object), std::move(particle));
		std::pair<WorldTransform, decltype(objectPair)> objectPair2(std::move(objectTransform), std::move(objectPair));
		objects_.push_back(std::move(objectPair2));
	}



}
