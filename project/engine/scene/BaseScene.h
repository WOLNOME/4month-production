#pragma once
#include "SceneLight.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "Audio.h"
#include "Input.h"
#include "LineDrawer.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "ParticleCommon.h"
#include "LineDrawerCommon.h"
#include "SpriteCommon.h"
#include <cstdint>
#include <memory>
class SceneManager;
class BaseScene
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseScene() = default;
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();
	/// <summary>
	/// 終了時
	/// </summary>
	virtual void Finalize();
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();
	
protected:
	//シーンマネージャー
	SceneManager* sceneManager_ = nullptr;
	//シーンライト
	std::unique_ptr<SceneLight> sceneLight_ = nullptr;

};

