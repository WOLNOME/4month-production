#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <list>
#include <memory>
#include <numbers>
#include "json.hpp"
#include "Model.h"
#include "MyMath.h"

using json = nlohmann::json;

class BaseCamera;
//パーティクル
class Particle
{
public:
	//座標変換行列データ
	struct ParticleForGPU {
		Matrix4x4 World;
		Vector4 color;
	};
	//モデルリソース作成用データ型
	struct ParticleResource {
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
		ParticleForGPU* instancingData;
		uint32_t srvIndex;
	};
	//エフェクト構造体
	struct EffectData {
		TransformEuler transform;
		Vector3 velocity;
		Vector4 color;
		float lifeTime;
		float currentTime;
	};
	//エミッター構造体
	struct Emitter {
		TransformEuler transform;//エミッターのトランスフォーム
		uint32_t count;//発生させるエフェクトの数
		float frequency;//発生頻度
		float frequencyTime;//頻度用時刻
	};
	//フィールド
	struct AccelerationField
	{
		Vector3 acceleration;
		AABB area;
		bool isActive;
	};
public://メンバ関数
	~Particle();
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="filePath">使用するパーティクルの名前(.jsonは省略)</param>
	void Initialize(const std::string& name);
	void Draw(const BaseCamera& camera, Emitter& emitter, AccelerationField* field = nullptr);
private://メンバ関数(非公開)
	//パーティクルリソース作成関数
	ParticleResource MakeParticleResource();
	//SRVの設定
	void SettingSRV();
	//パーティクルの生成
	EffectData MakeNewParticle(const Vector3& translate);
	//エミット
	std::list<EffectData> Emit(const Emitter& emitter);

public://getter
	const json& GetParam() { return param_; }
public://setter

private://メンバ変数
	//モデル(見た目)
	Model* model_;
	//パーティクル用リソース
	ParticleResource particleResource_;

	//各インスタンシング用書き換え情報
	std::list<EffectData> effects_;
	//δtの定義
	const float kDeltaTime = 1.0f / 60.0f;
	//ビルボードのオンオフ
	bool isBillboard = true;

private://パラメーター
	//各エフェクトのパラメーター
	json param_;
	//エミッター
	Emitter emitter_;

};