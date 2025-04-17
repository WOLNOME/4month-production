#include "FarceTitle.h"

void FarceTitle::Initialize(BaseCamera* _camera, SceneLight* _light) {
	camera_ = _camera;
	light_ = _light;
	//天球
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();
	//地面
	ground_.first.Initialize();
	ground_.first.scale_ = { 0.1f,1.0f,0.1f };
	ground_.second = std::make_unique<Object3d>();
	ground_.second->InitializeModel("ground");

	//メンバ変数
	scenario_ = Scenario::kEnemyRelax1;
	timer_ = 0.0f;

	//最初のシナリオの準備
	WorldTransform enemyTransform;
	enemyTransform.Initialize();
	auto enemy = std::make_unique<Object3d>();
	enemy->InitializeModel("enemy");
	enemies_.emplace_back(std::move(enemyTransform), std::move(enemy));

}

void FarceTitle::Finalize() {
}

void FarceTitle::Update() {
	//時間の更新
	timer_ += kDeltaTime;
	//各シナリオの更新
	switch (scenario_) {
	case Scenario::kEnemyRelax1:	//一回目の敵がくつろいでいる
		//カメラワーク
		if (timer_ < cameraEndTime) {
			TransformEuler cameraTransform;
			cameraTransform.scale = { 1.0f, 1.0f, 1.0f };
			cameraTransform.rotate = MyMath::Lerp(startCamera_.rotate, endCamera_.rotate, MyMath::EaseInBounce(timer_ / cameraEndTime));
			cameraTransform.translate = MyMath::Lerp(startCamera_.translate, endCamera_.translate, MyMath::EaseInBounce(timer_ / cameraEndTime));
			camera_->SetTranslate(cameraTransform.translate);
			camera_->SetRotate(cameraTransform.rotate);
		}
		else {
			camera_->SetTranslate(endCamera_.translate);
			camera_->SetRotate(endCamera_.rotate);
		}

		//制限時間になったら次のシナリオへ
		if (timer_ >= time_[(int)Scenario::kEnemyRelax1]) {
			timer_ = 0.0f;
			scenario_ = Scenario::kPlayerAttack1;
		}
		break;
	case Scenario::kPlayerAttack1:	//一回目のプレイヤーの攻撃
		//制限時間になったら次のシナリオへ
		if (timer_ >= time_[(int)Scenario::kPlayerAttack1]) {
			timer_ = 0.0f;
			scenario_ = Scenario::kPlayerRelax1;
		}
		break;
	case Scenario::kPlayerRelax1:	//一回目のプレイヤーがくつろいでいる
		//制限時間になったら次のシナリオへ
		if (timer_ >= time_[(int)Scenario::kPlayerRelax1]) {
			timer_ = 0.0f;
			scenario_ = Scenario::kEnemyAttack1;
		}
		break;
	case Scenario::kEnemyAttack1:	//一回目のエネミーの攻撃
		//制限時間になったら次のシナリオへ
		if (timer_ >= time_[(int)Scenario::kEnemyAttack1]) {
			timer_ = 0.0f;
			scenario_ = Scenario::kEnemyRelax2;
		}
		break;
	case Scenario::kEnemyRelax2:	//二回目の敵がくつろいでいる
		//制限時間になったら次のシナリオへ
		if (timer_ >= time_[(int)Scenario::kEnemyRelax2]) {
			timer_ = 0.0f;
			scenario_ = Scenario::kPlayerAttack2;
		}
		break;
	case Scenario::kPlayerAttack2:	//二回目のプレイヤーの攻撃
		//制限時間になったら次のシナリオへ
		if (timer_ >= time_[(int)Scenario::kPlayerAttack2]) {
			timer_ = 0.0f;
			scenario_ = Scenario::kPlayerRelax2;
		}
		break;
	case Scenario::kPlayerRelax2:	//二回目のプレイヤーがくつろいでいる
		//制限時間になったら次のシナリオへ
		if (timer_ >= time_[(int)Scenario::kPlayerRelax2]) {
			timer_ = 0.0f;
			scenario_ = Scenario::kEnemyAttack2;
		}
		break;
	case Scenario::kEnemyAttack2:	//二回目のエネミーの攻撃
		//制限時間になったら次のシナリオへ
		if (timer_ >= time_[(int)Scenario::kEnemyAttack2]) {
			timer_ = 0.0f;
			scenario_ = Scenario::kEnemyRelax3;
		}
		break;
	case Scenario::kEnemyRelax3:	//三回目の敵がくつろいでいる
		//制限時間になったら次のシナリオへ
		if (timer_ >= time_[(int)Scenario::kEnemyRelax3]) {
			timer_ = 0.0f;
			scenario_ = Scenario::kPlayerAttack3;
		}
		break;
	case Scenario::kPlayerAttack3:	//三回目のプレイヤーの攻撃
		//制限時間になったら次のシナリオへ
		if (timer_ >= time_[(int)Scenario::kPlayerAttack3]) {
			timer_ = 0.0f;
			scenario_ = Scenario::kNothing;
		}
		break;
	case Scenario::kNothing:	//何も起こらない
		//制限時間になったらタイトルシーンを再びやり直す

		break;
	default:
		break;
	}



	//全てのプレイヤーのワールドトランスフォームを更新
	for (auto& player : players_) {
		player.first.UpdateMatrix();
	}
	//全てのエネミーのワールドトランスフォームを更新
	for (auto& enemy : enemies_) {
		enemy.first.UpdateMatrix();
	}
	//天球のワールドトランスフォームを更新
	skydome_->Update();
	//地面のワールドトランスフォームを更新
	ground_.first.UpdateMatrix();
}

void FarceTitle::DrawObject() {
	//天球
	skydome_->Draw(*camera_);
	//地面
	ground_.second->Draw(ground_.first, *camera_);
	//全てのプレイヤーの描画
	for (auto& player : players_) {
		player.second->Draw(player.first, *camera_, light_);
	}
	//全てのエネミーの描画
	for (auto& enemy : enemies_) {
		enemy.second->Draw(enemy.first, *camera_, light_);
	}
}

void FarceTitle::TextDraw() {
}
