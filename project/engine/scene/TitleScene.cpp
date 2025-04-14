#include "TitleScene.h"
#include "SceneManager.h"
#include "application/appCollider/AppCollisionManager.h"

void TitleScene::Initialize() {
	//シーン共通の初期化
	BaseScene::Initialize();

	input_ = Input::GetInstance();

	//生成と初期化
	textureHandleUI_ = TextureManager::GetInstance()->LoadTexture("UI_SPACE.png");
	spriteUI_ = std::make_unique<Sprite>();
	spriteUI_->Initialize(textureHandleUI_);

	//カメラの生成と初期化
	camera = std::make_unique<DevelopCamera>();
	camera->Initialize();
	camera->SetTranslate({ 0.0f,20.0f,-20.0f });
	camera->SetRotate({ 0.6f,0.0f,0.0f });
	//パーティクルマネージャーにカメラをセット
	ParticleManager::GetInstance()->SetCamera(camera.get());

	title_ = std::make_unique<TextWrite>();
	title_->Initialize("TITLE");
	title_->SetParam({ 320.0f,100.0f }, Font::OnionScript, 80.0f, { 1,1,0,1 });
	title_->SetEdgeParam({ 1,0,0,1 }, 5.0f, 0.0f, true);

	//演出に使うプレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->SetPlayerPos({ 0.0f,1.0f,0.0f });

	//フィールド
	field_ = std::make_unique<Field>();
	field_->Initialize();

	//当たり判定
	appCollisionManager_ = AppCollisionManager::GetInstance();
	appCollisionManager_->Initialize();
}

void TitleScene::Finalize() {
}

void TitleScene::Update() {
	if (player_->IsDead()) {
		sceneManager_->SetNextScene("STAGESELECT");
	}

	//カメラの更新
	camera->Update();

#ifdef _DEBUG
	ImGui::Begin("scene");
	ImGui::Text("%s", "TITLE");
	/*======カメラ========*/
	//位置
	Vector3 cameraTranslate = camera->GetTranslate();
	ImGui::DragFloat3("cameraTranslate", &cameraTranslate.x, 0.1f);
	camera->SetTranslate(cameraTranslate);
	//回転
	Vector3 cameraRotate = camera->GetRotate();
	ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.1f);
	camera->SetRotate(cameraRotate);

	/*======プレイヤー======*/
	//プレイヤーの位置
	Vector3 playerPos = player_->GetPosition();
	ImGui::DragFloat3("playerTranslate", &playerPos.x, 0.1f);
	player_->SetPosition(playerPos);
	//プレイヤーのスケール
	Vector3 playerScale = player_->GetScale();
	ImGui::DragFloat3("playerScale", &playerScale.x, 0.1f);
	player_->SetScale(playerScale);
	//プレイヤーの回転
	Vector3 playerRotate = player_->GetRotation();
	ImGui::DragFloat3("playerRotate", &playerRotate.x, 0.1f);
	player_->SetRotation(playerRotate);

	/*======フィールド======*/
	//フィールドの位置
	Vector3 fieldPos = field_->GetPosition();
	ImGui::DragFloat3("fieldTranslate", &fieldPos.x, 0.1f);
	field_->SetPosition(fieldPos);
	//フィールドのスケール
	Vector3 fieldScale = field_->GetScale();
	ImGui::DragFloat3("fieldScale", &fieldScale.x, 0.1f);
	field_->SetScale(fieldScale);
	//フィールドの回転
	Vector3 fieldRotate = field_->GetRotation();
	ImGui::DragFloat3("fieldRotate", &fieldRotate.x, 0.1f);
	field_->SetRotation(fieldRotate);

	ImGui::End();
	//タイトルテキスト用のImGui
	title_->DebugWithImGui();

#endif // _DEBUG

	spriteUI_->Update();

	//プレイヤーの更新
	player_->Update();
	PlayerTackleCharge();
	//フィールドの更新
	field_->Update();
	//当たり判定
	appCollisionManager_->CheckAllCollision();
}

void TitleScene::Draw() {
	//3Dモデルの共通描画設定
	Object3dCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓モデル描画開始↓↓↓↓
	///------------------------------///

	player_->Draw(*camera.get());

	field_->Draw(*camera.get());

	///------------------------------///
	///↑↑↑↑モデル描画終了↑↑↑↑
	///------------------------------///


	//線描画共通描画設定
	LineDrawerCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓線描画開始↓↓↓↓
	///------------------------------///



	///------------------------------///
	///↑↑↑↑線描画終了↑↑↑↑
	///------------------------------///

	//スプライトの共通描画設定
	SpriteCommon::GetInstance()->SettingCommonDrawing();

	///------------------------------///
	///↓↓↓↓スプライト描画開始↓↓↓↓
	///------------------------------///

	spriteUI_->Draw();

	///------------------------------///
	///↑↑↑↑スプライト描画終了↑↑↑↑
	///------------------------------///

}

void TitleScene::TextDraw() {
	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///

	//タイトルテキスト
	title_->WriteText(L"ふえるぶつかり屋");


	///------------------------------///
	///↑↑↑↑テキスト描画終了↑↑↑↑
	///------------------------------///
}

void TitleScene::PlayerTackleCharge()
{

	// プレイヤーの攻撃フラグが立っているとき
	if (player_->IsChargeMax())
	{
		charge_ += 1.0f / 60.0f;
	}

	// チャージが最大値に達したら
	if (charge_ >= kChargeMax)
	{
		player_->SetIsChargeMax(false);
		charge_ = 0.0f;
	}
}

