#include "StageSelectObject.h"

#include "TextureManager.h"
#include "ImGuiManager.h"
#include <random>

void StageSelectObject::Initialize(const std::string& modelFilePath, const std::string& textureFilePath, int stageNum) {
	input_ = Input::GetInstance();

	// ワールドトランスフォームの初期化
	wtField_.Initialize();
	wtField_.scale_ = { 2.0f, 0.1f, 2.0f };
	wtField_.rotate_ = { 0.0f, 0.0f, 0.0f };

	// テクスチャの読み込み
	textureHandleField_ = TextureManager::GetInstance()->LoadTexture(textureFilePath);

	// モデルの初期化
	field_ = std::make_unique<Object3d>();
	field_->InitializeModel(modelFilePath);

	isMove_ = false;

	//このインスタンスの担当番号
	stageNum_ = (StageNum)stageNum;
	//このインスタンスで使われるエネミーを番号ごとに初期化分けする
	switch (stageNum_) {
	case StageNum::Tutorial:
		//タックルエネミーを2体配置
		for (int i = 0; i < 2; i++) {
			//ワールドトランスフォームの初期化
			WorldTransform enemyTransform;
			enemyTransform.Initialize();
			enemyTransform.translate_ = { i * 1.0f - 0.5f,2.0f,i * 1.0f - 0.5f };
			enemyTransform.scale_ = { 0.2f,4.0f,0.2f };
			enemyTransform.parent_ = &wtField_;
			//モデルの初期化
			std::unique_ptr<Object3d> enemy = std::make_unique<Object3d>();
			enemy->InitializeModel("enemy");
			//速度の初期化
			Vector3 velocity = { 0.0f,0.0f,0.0f };
			//登録
			enemies_.emplace_back(std::move(enemyTransform), std::move(enemy));
			enemyVelocity_.push_back(velocity);
		}

		break;
	case StageNum::Stage1:
		//タックルエネミーを2体配置
		for (int i = 0; i < 2; i++) {
			//ワールドトランスフォームの初期化
			WorldTransform enemyTransform;
			enemyTransform.Initialize();
			enemyTransform.translate_ = { i * 1.0f - 0.5f,2.0f,i * 1.0f - 0.5f };
			enemyTransform.scale_ = { 0.2f,4.0f,0.2f };
			enemyTransform.parent_ = &wtField_;
			//モデルの初期化
			std::unique_ptr<Object3d> enemy = std::make_unique<Object3d>();
			enemy->InitializeModel("enemy");
			//速度の初期化
			Vector3 velocity = { 0.0f,0.0f,0.0f };
			//登録
			enemies_.emplace_back(std::move(enemyTransform), std::move(enemy));
			enemyVelocity_.push_back(velocity);
		}

		break;
	case StageNum::Stage2:
		//タックルエネミーとファンエネミーを1体ずつ配置
		for (int i = 0; i < 2; i++) {
			//ワールドトランスフォームの初期化
			WorldTransform enemyTransform;
			enemyTransform.Initialize();
			enemyTransform.translate_ = { i * 1.0f - 0.5f,2.0f,i * 1.0f - 0.5f };
			enemyTransform.scale_ = { 0.2f,4.0f,0.2f };
			enemyTransform.parent_ = &wtField_;
			//モデルの初期化
			std::unique_ptr<Object3d> enemy = std::make_unique<Object3d>();
			std::string name = "enemy";
			if (i == 0) name = "Fan";
			enemy->InitializeModel(name);
			//速度の初期化
			Vector3 velocity = { 0.0f,0.0f,0.0f };
			//登録
			enemies_.emplace_back(std::move(enemyTransform), std::move(enemy));
			enemyVelocity_.push_back(velocity);
		}
		//壁を配置
		{
			//ワールドトランスフォームの初期化
			WorldTransform gimmickTransform;
			gimmickTransform.Initialize();
			gimmickTransform.translate_ = { 0.0f,2.5f,0.0f };
			gimmickTransform.scale_ = { 0.15f,4.0f,0.8f };
			gimmickTransform.parent_ = &wtField_;
			//モデルの初期化
			std::unique_ptr<Object3d> gimmick = std::make_unique<Object3d>();
			gimmick->InitializeModel("obstacle");
			//登録
			gimmicks_.emplace_back(std::move(gimmickTransform), std::move(gimmick));
		}

		break;
	case StageNum::Stage3:
		//タックルエネミーとフリーズエネミーを1体ずつ配置
		for (int i = 0; i < 2; i++) {
			//ワールドトランスフォームの初期化
			WorldTransform enemyTransform;
			enemyTransform.Initialize();
			enemyTransform.translate_ = { i * 1.0f - 0.5f,2.0f,i * 1.0f - 0.5f };
			enemyTransform.scale_ = { 0.2f,4.0f,0.2f };
			enemyTransform.parent_ = &wtField_;
			//モデルの初期化
			std::unique_ptr<Object3d> enemy = std::make_unique<Object3d>();
			std::string name = "enemy";
			if (i == 1) name = "Freeze";
			enemy->InitializeModel(name);
			//速度の初期化
			Vector3 velocity = { 0.0f,0.0f,0.0f };
			//登録
			enemies_.emplace_back(std::move(enemyTransform), std::move(enemy));
			enemyVelocity_.push_back(velocity);
		}
		//氷床を配置
		{
			//ワールドトランスフォームの初期化
			WorldTransform gimmickTransform;
			gimmickTransform.Initialize();
			gimmickTransform.translate_ = { 0.0f,2.01f,0.0f };
			gimmickTransform.scale_ = { 1.0f,1.0f,1.0f };
			gimmickTransform.parent_ = &wtField_;
			//モデルの初期化
			std::unique_ptr<Object3d> gimmick = std::make_unique<Object3d>();
			gimmick->InitializeModel("iceFloor");
			//登録
			gimmicks_.emplace_back(std::move(gimmickTransform), std::move(gimmick));
		}
		break;
	case StageNum::Stage4:
		//タックルエネミーとファンエネミーを1体ずつ配置
		for (int i = 0; i < 2; i++) {
			//ワールドトランスフォームの初期化
			WorldTransform enemyTransform;
			enemyTransform.Initialize();
			enemyTransform.translate_ = { i * 1.0f - 0.5f,2.0f,i * 1.0f - 0.5f };
			enemyTransform.scale_ = { 0.2f,4.0f,0.2f };
			enemyTransform.parent_ = &wtField_;
			//モデルの初期化
			std::unique_ptr<Object3d> enemy = std::make_unique<Object3d>();
			std::string name = "enemy";
			if (i == 0) name = "Fan";
			enemy->InitializeModel(name);
			//速度の初期化
			Vector3 velocity = { 0.0f,0.0f,0.0f };
			//登録
			enemies_.emplace_back(std::move(enemyTransform), std::move(enemy));
			enemyVelocity_.push_back(velocity);
		}
		//反射壁を配置
		{
			//ワールドトランスフォームの初期化
			WorldTransform gimmickTransform;
			gimmickTransform.Initialize();
			gimmickTransform.translate_ = { 0.0f,2.5f,0.0f };
			gimmickTransform.scale_ = { 0.15f,4.0f,0.8f };
			gimmickTransform.parent_ = &wtField_;
			//モデルの初期化
			std::unique_ptr<Object3d> gimmick = std::make_unique<Object3d>();
			gimmick->InitializeModel("bumper");
			//登録
			gimmicks_.emplace_back(std::move(gimmickTransform), std::move(gimmick));
		}
		break;
	case StageNum::Stage5:
		//タックルエネミーとファンエネミーとフリーズエネミーを1体ずつ配置
		for (int i = 0; i < 3; i++) {
			//ワールドトランスフォームの初期化
			WorldTransform enemyTransform;
			enemyTransform.Initialize();
			enemyTransform.translate_ = { i * 1.0f - 0.5f,2.0f,i * 1.0f - 0.5f };
			if (i == 2) enemyTransform.translate_ = { -0.5f,2.0f,0.5f };
			enemyTransform.scale_ = { 0.2f,4.0f,0.2f };
			enemyTransform.parent_ = &wtField_;
			//モデルの初期化
			std::unique_ptr<Object3d> enemy = std::make_unique<Object3d>();
			std::string name = "enemy";
			if (i == 0) name = "Fan";
			if (i == 1) name = "Freeze";
			enemy->InitializeModel(name);
			//速度の初期化
			Vector3 velocity = { 0.0f,0.0f,0.0f };
			//登録
			enemies_.emplace_back(std::move(enemyTransform), std::move(enemy));
			enemyVelocity_.push_back(velocity);
		}
		//氷床を配置
		{
			//ワールドトランスフォームの初期化
			WorldTransform gimmickTransform;
			gimmickTransform.Initialize();
			gimmickTransform.translate_ = { 0.0f,2.01f,0.0f };
			gimmickTransform.scale_ = { 1.0f,1.0f,1.0f };
			gimmickTransform.parent_ = &wtField_;
			//モデルの初期化
			std::unique_ptr<Object3d> gimmick = std::make_unique<Object3d>();
			gimmick->InitializeModel("iceFloor");
			//登録
			gimmicks_.emplace_back(std::move(gimmickTransform), std::move(gimmick));
		}
		//反射壁を配置
		{
			//ワールドトランスフォームの初期化
			WorldTransform gimmickTransform;
			gimmickTransform.Initialize();
			gimmickTransform.translate_ = { 0.0f,2.5f,0.0f };
			gimmickTransform.scale_ = { 0.15f,4.0f,0.8f };
			gimmickTransform.parent_ = &wtField_;
			//モデルの初期化
			std::unique_ptr<Object3d> gimmick = std::make_unique<Object3d>();
			gimmick->InitializeModel("bumper");
			//登録
			gimmicks_.emplace_back(std::move(gimmickTransform), std::move(gimmick));
		}
		break;
	case StageNum::StageNumMax:
		break;
	default:
		break;
	}
}

void StageSelectObject::Finalize() {
	field_.reset();
}

void StageSelectObject::Update() {
	//エネミーの更新
	EnemyJump();


	// ワールドトランスフォームの更新
	wtField_.UpdateMatrix();
	wtField_.rotate_.y += 0.01f;
	//全てのエネミーのワールドトランスフォームを更新
	for (auto& enemy : enemies_) {
		enemy.first.UpdateMatrix();
	}
	//全てのギミックのワールドトランスフォームを更新
	for (auto& gimmick : gimmicks_) {
		gimmick.first.UpdateMatrix();
	}

	StageSelect();

}

void StageSelectObject::Draw(BaseCamera _camera) {
	field_->Draw(wtField_, _camera, nullptr, textureHandleField_);
	//登録されている全てのエネミーを描画
	for (auto& enemy : enemies_) {
		enemy.second->Draw(enemy.first, _camera);
	}
	//登録されている全てのギミックを描画
	for (auto& gimmick : gimmicks_) {
		gimmick.second->Draw(gimmick.first, _camera);
	}

}

void StageSelectObject::ImGuiDraw() {
	// ImGuiによるデバッグ描画
	ImGui::Begin("StageSelectObject");

	ImGui::SliderFloat3("FieldPos", &wtField_.translate_.x, -50.0f, 50.0f);
	ImGui::SliderFloat3("FieldSize", &wtField_.scale_.x, 0.0f, 50.0f);

	ImGui::Text("nowStage_ : %d", nowStage_);

	ImGui::End();
}

void StageSelectObject::StageSelect() {
	if (!isMove_) {
		if (input_->TriggerKey(DIK_D) && nowStage_ < 5) {
			isMoveLeft_ = true;
			isMove_ = true;
		}

		if (input_->TriggerKey(DIK_A) && nowStage_ > 0) {
			isMoveRight_ = true;
			isMove_ = true;
		}
	}

	// 左に移動
	if (isMoveLeft_) {
		wtField_.translate_.x -= 0.25f;
		moveLength_ -= 0.25f;
	}

	// 右に移動
	if (isMoveRight_) {
		wtField_.translate_.x += 0.25f;
		moveLength_ += 0.25f;
	}

	// 移動量が一定以上になったら移動を止める
	if (moveLength_ <= -10.0f or moveLength_ >= 10.0f) {
		isMoveLeft_ = false;
		isMoveRight_ = false;
		isMove_ = false;
		moveLength_ = 0.0f;
	}
}

void StageSelectObject::EnemyJump() {
	//エネミーのY座標を上昇させる
	float gravity = -100.0f;			//重力値
	float enemyJumpHeight = 2.0f;		//床の高さ
	int enemyNum = 0;					//エネミーの番号
	//ランダム
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	for (auto& enemy : enemies_) {
		//重力値を速度に加算
		enemyVelocity_[enemyNum].y += gravity * kDeltaTime;
		//エネミーの座標を更新
		enemy.first.translate_.y += enemyVelocity_[enemyNum].y * kDeltaTime;
		//エネミーの座標が床の高さよりも低い場合、ランダムの値でジャンプ
		if (enemy.first.translate_.y < enemyJumpHeight) {
			//エネミーのY座標を上昇させる
			std::uniform_real_distribution<float> rand(30.0f, 35.0f);
			enemyVelocity_[enemyNum].y = rand(engine);
		}
		//次のエネミーへ
		enemyNum++;
	}
}
