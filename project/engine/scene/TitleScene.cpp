#include "TitleScene.h"
#include "SceneManager.h"
#include "application/MathUtils.h"
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
    camera->SetTranslate({ 0.0f, 10.0f, -30.0f }); // カメラの位置を調整
    camera->SetRotate({ 0.3f, 0.0f, 0.0f }); // カメラの回転を調整
    //パーティクルマネージャーにカメラをセット
    ParticleManager::GetInstance()->SetCamera(camera.get());

    title_ = std::make_unique<TextWrite>();
    title_->Initialize("TITLE");
    title_->SetParam({ 320.0f,50.0f }, Font::OnionScript, 80.0f, { 1,1,0,1 });
    title_->SetEdgeParam({ 1,0,0,1 }, 5.0f, 0.0f, true);

    //演出に使うプレイヤー
    player_ = std::make_unique<Object3d>();
    player_->InitializeModel("player");
    playerTransform_.Initialize();
    playerTransform_.translate_ = { -10.0f, 1.0f, 0.0f };
    playerVelocity_ = initialPlayerVelocity_;

    //演出に使う敵
    enemy_ = std::make_unique<Object3d>();
    enemy_->InitializeModel("enemy");
    enemyTransform_.Initialize();
    enemyTransform_.translate_ = { 0.0f, 1.0f, 0.0f };

    //フィールド
    field_ = std::make_unique<Field>();
    field_->Initialize();
    field_->SetScale({ 23.0f, 1.0f,26.0f });

    //当たり判定
    appCollisionManager_ = AppCollisionManager::GetInstance();
    appCollisionManager_->Initialize();

    //パーティクルの初期化
	hitEffect_ = std::make_unique<Particle>();

	backgroundEffect_ = std::make_unique<Particle>();
	backgroundEffect_->Initialize("background", "basic");
	backgroundEffect_->emitter_.isGravity = true;
	backgroundEffect_->emitter_.gravity = 5.0f;
	backgroundEffect_->emitter_.isPlay = true;
	backgroundEffect_->emitter_.transform.translate = { 0.0f, 0.0f, 26.0f };
	backgroundEffect_->emitter_.transform.scale = { 24.0f, 1.0f, 1.0f };

    //bgm
	bgm_ = std::make_unique<Audio>();
	bgm_->Initialize("title/bgm.wav");
    bgm_->Play(true,0.5f);
	//タップしたときの効果音
	tapSE_ = std::make_unique<Audio>();
	tapSE_->Initialize("soundeffect/tap.wav");
}

void TitleScene::Finalize() {

    field_->Finalize();

    hitEffect_->emitter_.isPlay = false;
    hitEffect_.reset();
    backgroundEffect_->emitter_.isPlay = false;
	backgroundEffect_.reset();

	bgm_->Stop();
	bgm_.reset();

	tapSE_->Stop();
	tapSE_.reset();
}

void TitleScene::Update() {
    //カットシーンの更新
    if (isCutscenePlaying_) {
        UpdateCutscene();
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

	/*======パーティクル======*/
	ImGui::DragFloat3("particleTranslate", &backgroundEffect_->emitter_.transform.translate.x, 0.1f);
	ImGui::DragFloat3("particleScale", &backgroundEffect_->emitter_.transform.scale.x, 0.1f);
	ImGui::DragFloat3("particleRotate", &backgroundEffect_->emitter_.transform.rotate.x, 0.1f);

    /*======プレイヤー======*/
    //プレイヤーの位置
    ImGui::DragFloat3("playerTranslate", &playerTransform_.translate_.x, 0.1f);
    //プレイヤーのスケール
    ImGui::DragFloat3("playerScale", &playerTransform_.scale_.x, 0.1f);
    //プレイヤーの回転
    ImGui::DragFloat3("playerRotate", &playerTransform_.rotate_.x, 0.1f);

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
    playerTransform_.UpdateMatrix();
    //敵の更新
    enemyTransform_.UpdateMatrix();
    //フィールドの更新
    field_->Update();
    //当たり判定
    appCollisionManager_->CheckAllCollision();

    // ロゴのフェードイン・フェードアウト
    if (isFadingIn_) {
        logoAlpha_ += 0.01f;
        if (logoAlpha_ >= 1.0f) {
            logoAlpha_ = 1.0f;
            isFadingIn_ = false;
        }
    }
    else {
        logoAlpha_ -= 0.01f;
        if (logoAlpha_ <= 0.0f) {
            logoAlpha_ = 0.0f;
            isFadingIn_ = true;
        }
    }
    Vector4 titleColor = { 1.0f, 1.0f, 0.0f, logoAlpha_ }; // タイトルの色
    Vector4 titleOutlineColor = { 1.0f, 0.0f, 0.0f, logoAlpha_ }; // タイトルのアウトラインの色
    // タイトルの色を更新
    title_->SetColor(titleColor);
    title_->SetEdgeColor(titleOutlineColor);

    //次のシーンへ
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->SetNextScene("STAGESELECT");
		tapSE_->Play(false, 0.5f);
	}

}

void TitleScene::Draw() {
    //3Dモデルの共通描画設定
    Object3dCommon::GetInstance()->SettingCommonDrawing();

    ///------------------------------///
    ///↓↓↓↓モデル描画開始↓↓↓↓///
    ///------------------------------///

    player_->Draw(playerTransform_, *camera.get());
    enemy_->Draw(enemyTransform_, *camera.get());
    field_->Draw(*camera.get());

    ///------------------------------///
    ///↑↑↑↑モデル描画終了↑↑↑↑///
    ///------------------------------///


    //線描画共通描画設定
    LineDrawerCommon::GetInstance()->SettingCommonDrawing();

    ///------------------------------///
    ///↓↓↓↓線描画開始↓↓↓↓///
    ///------------------------------///



    ///------------------------------///
    ///↑↑↑↑線描画終了↑↑↑↑///
    ///------------------------------///

    //スプライトの共通描画設定
    SpriteCommon::GetInstance()->SettingCommonDrawing();

    ///------------------------------///
    ///↓↓↓↓スプライト描画開始↓↓↓↓///
    ///------------------------------///

    spriteUI_->Draw();

    ///------------------------------///
    ///↑↑↑↑スプライト描画終了↑↑↑↑///
    ///------------------------------///

}

void TitleScene::TextDraw() {
    ///------------------------------///
    ///↑↑↑↑テキスト描画終了↑↑↑↑///
    ///------------------------------///

    //タイトルテキスト
    title_->WriteText(L"ふえるぶつかり屋");

    ///------------------------------///
    ///↑↑↑↑テキスト描画終了↑↑↑↑///
    ///------------------------------///
}

void TitleScene::UpdateCutscene()
{
    // 時間を経過させる
    cutsceneTime_ += 1.0f / 60.0f;

    switch (cutsceneState_) {
    case CutsceneState::ZoomInOnPlayer:
	{
		// カメラをプレイヤーにズームイン
		Vector3 targetCameraPos = playerTransform_.translate_ + Vector3(0.0f, 10.0f, -30.0f);
		camera->SetTranslate(Lerp(camera->GetTranslate(), targetCameraPos, 0.1f)); // カメラの位置を調整
		camera->SetRotate(Lerp(camera->GetRotate(), { 0.3f, 0.0f, 0.0f }, 0.05f)); // カメラの回転を調整
		if (cutsceneTime_ >= cutsceneDuration_ * 0.5) {
			cutsceneTime_ = 0.0f;
			cutsceneState_ = CutsceneState::PlayerRunningIn;
		}
	}
   
    break;
    case CutsceneState::PlayerRunningIn:
    {
        playerVelocity_ *= friction_; // 摩擦を適用
        playerTransform_.translate_ += playerVelocity_ * (1.0f / 60.0f);
        Vector3 targetCameraPos = playerTransform_.translate_ + Vector3(0.0f, 10.0f, -30.0f);
        camera->SetTranslate(Lerp(camera->GetTranslate(), targetCameraPos, 0.06f)); // カメラの位置を調整
        camera->SetRotate({ 0.3f, 0.0f, 0.0f }); // カメラの回転を調整
        if (CheckCollision()) {
            cutsceneTime_ = 0.0f;
            cutsceneState_ = CutsceneState::Hit;
        }
    }
    break;
    case CutsceneState::PlayerPreparing:
    {
        // プレイヤーが構える
        playerTransform_.rotate_ = { 0.0f, 0.0f, 0.0f };
        if (cutsceneTime_ >= 0.1f) {
            cutsceneTime_ = 0.0f;
            cutsceneState_ = CutsceneState::Hit;
        }
    }
    break;
    case CutsceneState::Hit:
    {
        //パーティクルの生成
        hitEffect_->Initialize("hitOnTitleScene", "star");
        hitEffect_->emitter_.isPlay = true;
		hitEffect_->emitter_.transform.translate = enemyTransform_.translate_;
        hitEffect_->emitter_.isGravity = true;
        hitEffect_->emitter_.gravity = -150.0f;
        hitEffect_->emitter_.isBound = true;
        hitEffect_->emitter_.repulsion = 0.8f;
        hitEffect_->emitter_.floorHeight = 0.0f;
        // 画面フラッシュやカメラぶれを実装
        Vector3 targetCameraPos = enemyTransform_.translate_ + Vector3(0.0f, 10.0f, -30.0f);
        camera->SetTranslate(Lerp(camera->GetTranslate(), targetCameraPos, 0.04f)); // カメラの位置を調整
        camera->SetRotate({ 0.3f, 0.0f, 0.0f }); // カメラの回転を調整
        if (cutsceneTime_ >= 0.2f) {
            cutsceneTime_ = 0.0f;
            cutsceneState_ = CutsceneState::EnemyFlyingOut;
            enemyVelocity_ = initialEnemyVelocity_; // 初速度を設定
        }
    }
    break;
    case CutsceneState::EnemyFlyingOut:
    {
        hitEffect_->emitter_.transform.translate = enemyTransform_.translate_;
        playerVelocity_ *= friction_; // 摩擦を適用
        playerTransform_.translate_ += playerVelocity_ * (1.0f / 60.0f);
        enemyVelocity_ *= friction_; // 摩擦を適用
        enemyTransform_.translate_ += enemyVelocity_ * (1.0f / 60.0f);
        Vector3 targetCameraPos = enemyTransform_.translate_ + Vector3(0.0f, 10.0f, -30.0f);
        camera->SetTranslate(Lerp(camera->GetTranslate(), targetCameraPos, 0.2f)); // カメラの位置を調整
        camera->SetRotate({ 0.3f, 0.0f, 0.0f }); // カメラの回転を調整
        if (cutsceneTime_ >= cutsceneDuration_) {
            cutsceneTime_ = 0.0f;
            cutsceneState_ = CutsceneState::ZoomIn;
			hitEffect_->emitter_.isPlay = false; // パーティクルを停止
        }
    }
    break;
   case CutsceneState::ZoomIn:
    {
	   // カメラをプレイヤーにズームイン
   		Vector3 targetCameraPos = playerTransform_.translate_ + Vector3(0.0f, 3.0f, -10.0f);
        camera->SetTranslate(Lerp(camera->GetTranslate(), targetCameraPos, 0.05f)); // カメラの位置を調整
        camera->SetRotate(Lerp(camera->GetRotate(), { 0.2f, 0.0f, 0.0f }, 0.05f)); // カメラの回転を調整
        if (cutsceneTime_ >= cutsceneDuration_ * 4.0f) {
            cutsceneTime_ = 0.0f;
            cutsceneState_ = CutsceneState::Reset;
        }
    }
    break;
    case CutsceneState::Reset:
    {
        // プレイヤーとエネミーの位置をリセット
        playerTransform_.translate_ = { -10.0f, 1.0f, 0.0f };
        enemyTransform_.translate_ = { 0.0f, 1.0f, 0.0f };
        playerVelocity_ = initialPlayerVelocity_; // 初速度を設定
        enemyVelocity_ = { 0.0f, 0.0f, 0.0f };
        cutsceneState_ = CutsceneState::ZoomOut;
    }
    break;
    case CutsceneState::ZoomOut:
    {
        // カメラを元の位置にズームアウト
        camera->SetTranslate(Lerp(camera->GetTranslate(), { 0.0f, 10.0f, -40.0f }, 0.05f)); // カメラの位置を調整
        camera->SetRotate(Lerp(camera->GetRotate(), { 0.2f, 0.0f, 0.0f }, 0.05f)); // カメラの回転を調整
        if (cutsceneTime_ >= cutsceneDuration_) {
            cutsceneTime_ = 0.0f;
            cutsceneState_ = CutsceneState::ShowTitle;
            waitTime_ = 0.0f; // 待機時間をリセット
        }
    }
    break;
    case CutsceneState::ShowTitle:
    {
        // 数秒間待機
        waitTime_ += 1.0f / 60.0f;
        if (waitTime_ >= waitDuration_) {
            ResetCutscene();
        }
    }
    break;
    }
}

void TitleScene::ResetCutscene()
{
    cutsceneTime_ = 0.0f;
    cutsceneState_ = CutsceneState::ZoomInOnPlayer;
    playerVelocity_ = initialPlayerVelocity_; // 初速度を設定
    enemyVelocity_ = { 0.0f, 0.0f, 0.0f };
    playerTransform_.translate_ = { -10.0f, 1.0f, 0.0f };
    enemyTransform_.translate_ = { 0.0f, 1.0f, 0.0f };
}

bool TitleScene::CheckCollision()
{
    // AABBでの衝突判定
    Vector3 playerMin = playerTransform_.translate_ - playerTransform_.scale_;
    Vector3 playerMax = playerTransform_.translate_ + playerTransform_.scale_;
    Vector3 enemyMin = enemyTransform_.translate_ - enemyTransform_.scale_;
    Vector3 enemyMax = enemyTransform_.translate_ + enemyTransform_.scale_;

    return (playerMin.x <= enemyMax.x && playerMax.x >= enemyMin.x) &&
        (playerMin.y <= enemyMax.y && playerMax.y >= enemyMin.y) &&
        (playerMin.z <= enemyMax.z && playerMax.z >= enemyMin.z);
}