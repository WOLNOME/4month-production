#include "StageSelectObject.h"

#include "TextureManager.h"
#include "ImGuiManager.h"

void StageSelectObject::Initialize(const std::string& modelFilePath, const std::string& textureFilePath)
{
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
}

void StageSelectObject::Finalize()
{
	field_.reset();
}

void StageSelectObject::Update()
{
    // ワールドトランスフォームの更新
    wtField_.UpdateMatrix();
	wtField_.rotate_.y += 0.01f; 

    StageSelect();

}

void StageSelectObject::Draw(BaseCamera _camera)
{
	field_->Draw(wtField_, _camera, nullptr, textureHandleField_);
}

void StageSelectObject::ImGuiDraw()
{
    // ImGuiによるデバッグ描画
    ImGui::Begin("StageSelectObject");

    ImGui::SliderFloat3("FieldPos", &wtField_.translate_.x, -50.0f, 50.0f);
    ImGui::SliderFloat3("FieldSize", &wtField_.scale_.x, 0.0f, 50.0f);

	ImGui::Text("nowStage_ : %d", nowStage_);

    ImGui::End();
}

void StageSelectObject::StageSelect()
{
    if (!isMove_)
    {
        if (input_->TriggerKey(DIK_D) && nowStage_ < 4)
        {
            isMoveLeft_ = true;
			isMove_ = true;
        }

        if (input_->TriggerKey(DIK_A) && nowStage_ > 0)
        {
            isMoveRight_ = true;
			isMove_ = true;
        }
    }

    // 左に移動
    if (isMoveLeft_)
    {
        wtField_.translate_.x -= 0.25f;
        moveLength_ -= 0.25f;
    }

	// 右に移動
    if (isMoveRight_)
    {
        wtField_.translate_.x += 0.25f;
        moveLength_ += 0.25f;
    } 
    
	// 移動量が一定以上になったら移動を止める
    if(moveLength_ <= -10.0f or moveLength_ >= 10.0f)
    {
		isMoveLeft_ = false;
		isMoveRight_ = false;
		isMove_ = false;
		moveLength_ = 0.0f;
    }
}
