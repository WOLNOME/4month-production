#pragma once
#include "BaseCamera.h"
#include "SceneLight.h"
#include "application/objects/Skydome/Skydome.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include "MyMath.h"
#include <list>
#include <memory>


class FarceTitle {
public://列挙型
	enum class Scenario {
		kEnemyRelax1,			//一回目の敵がくつろぎ
		kPlayerAttack1,			//一回目のプレイヤーの攻撃
		kPlayerRelax1,			//一回目のプレイヤーがくつろぎ
		kEnemyAttack1,			//一回目のエネミーの攻撃
		kEnemyRelax2,			//二回目の敵がくつろぎ
		kPlayerAttack2,			//二回目のプレイヤーの攻撃
		kPlayerRelax2,			//二回目のプレイヤーがくつろぎ
		kEnemyAttack2,			//二回目のエネミーの攻撃
		kEnemyRelax3,			//三回目の敵がくつろぎ
		kPlayerAttack3,			//三回目のプレイヤーの攻撃
		kNothing,				//何も起こらない

		kMaxNumScenario,		//シナリオの数
	};
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(BaseCamera* _camera, SceneLight* _light);
	/// <summary>
	/// 終了時
	/// </summary>
	void Finalize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// オブジェクト描画
	/// </summary>
	void DrawObject();
	/// <summary>
	/// テキスト描画
	/// </summary>
	void TextDraw();
private://借り物
	//カメラ
	BaseCamera* camera_ = nullptr;
	//ライト
	SceneLight* light_ = nullptr;

private://メンバ変数
	//天球
	std::unique_ptr<Skydome> skydome_ = nullptr;
	//地面
	std::pair<WorldTransform, std::unique_ptr<Object3d>> ground_ = {};

	//プレイヤー
	std::list<std::pair<WorldTransform, std::unique_ptr<Object3d>>> players_;
	//エネミー
	std::list<std::pair<WorldTransform, std::unique_ptr<Object3d>>> enemies_;


	//シナリオ
	Scenario scenario_;
	//時間関係
	float timer_;												//各シナリオの現在時間
	const float time_[(int)Scenario::kMaxNumScenario] = {		//各シナリオの制限時間
		10.0f,		//一回目の敵がくつろぎ
		10.0f,		//一回目のプレイヤーの攻撃
		10.0f,		//一回目のプレイヤーがくつろぎ
		10.0f,		//一回目のエネミーの攻撃
		10.0f,		//二回目の敵がくつろぎ
		10.0f,		//二回目のプレイヤーの攻撃
		10.0f,		//二回目のプレイヤーがくつろぎ
		10.0f,		//二回目のエネミーの攻撃
		10.0f,		//三回目の敵がくつろぎ
		10.0f,		//三回目のプレイヤーの攻撃
		10.0f,		//何も起こらない
	};

	//カメラワーク
	TransformEuler startCamera_ = {
		{ 1.0f, 1.0f, 1.0f },			//カメラのスケール
		{ 0.03f, 1.5f, 0.0f },			//カメラの回転
		{ -4.7f, 1.0f, 0.0f },			//カメラの位置
	};
	TransformEuler endCamera_ = {
		{ 1.0f, 1.0f, 1.0f },			//カメラのスケール
		{ 0.27f, 3.14f / 2.0f, 0.0f },	//カメラの回転
		{ -38.0f, 10.0f, 0.0f },		//カメラの位置
	};
	float cameraEndTime = time_[(int)Scenario::kEnemyRelax1] / 2.0f;

};

