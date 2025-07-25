#include "GameCamera.h"
#include <random>

void GameCamera::Initialize() {
	//基盤の初期化
	BaseCamera::Initialize();

	//シェイク変数初期化
	shakeOffset_ = { 0.0f,0.0f,0.0f };
}

void GameCamera::Update() {
	//揺れ幅の更新
	UpdateShake();
	//シェイクオフセット分加算
	transform.translate += shakeOffset_;
	//行列の更新
	UpdateMatrix();
	//シェイクオフセット分元に戻す
	transform.translate -= shakeOffset_;
	
}

void GameCamera::RegistShake(float time, float power) {
	//揺れのデータをとる
	ShakeData shakeData;
	shakeData.maxTime = time;
	shakeData.time = time;
	shakeData.maxPower = power;
	shakeData.power = power;
	//リストに登録
	shakeList_.push_back(shakeData);
}

void GameCamera::UpdateShake() {
	//オフセットを0で更新
	shakeOffset_ = { 0.0f, 0.0f, 0.0f };
	//リストに何もなければ終了
	if (shakeList_.empty()) return;
	//ローカル変数
	float usePower = 0.0f;
	//全てのリストを更新
	for (auto it = shakeList_.begin(); it != shakeList_.end();) {
		//揺れの大きさを線形補完で決める
		it->power = MyMath::Lerp(it->maxPower, 0.0f, 1.0f - (it->time / it->maxTime));
		//時間を減らす
		it->time -= kDeltaTime;
		//時間が0未満になったら削除
		if (it->time < 0.0f) {
			it = shakeList_.erase(it);
			//次の要素へ
			continue;
		}
		//揺れの大きさが大きいほうを使う
		if (usePower < it->power) {
			usePower = it->power;
		}
		//次の要素へ
		it++;
	}
	//最終的に決まった揺れの大きさを使ってオフセットを決める
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	std::uniform_real_distribution<float> dist(-usePower, usePower);
	shakeOffset_.x = dist(engine);
	shakeOffset_.y = dist(engine);
	shakeOffset_.z = dist(engine);
}
