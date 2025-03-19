#pragma once
#include "BaseCamera.h"
#include "Particle.h"
#include <d3d12.h>
#include <string>
#include <array>
#include <unordered_map>
#include <wrl.h>

enum class BlendMode {
	None,
	Normal,
	Add,
	Subtract,
	Multiply,
	Screen,
	Execlution,

	kMaxBlendModeNum,
};

class ParticleManager {
private://コンストラクタ等の隠蔽
	static ParticleManager* instance;

	ParticleManager() = default;//コンストラクタ隠蔽
	~ParticleManager() = default;//デストラクタ隠蔽
	ParticleManager(ParticleManager&) = delete;//コピーコンストラクタ封印
	ParticleManager& operator=(ParticleManager&) = delete;//コピー代入演算子封印
public:
	//シングルトンインスタンスの取得
	static ParticleManager* GetInstance();
public://メンバ関数
	//初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();
	//終了
	void Finalize();

	//パーティクルをコンテナに登録
	void RegisterParticle(const std::string& name, Particle* particle);

public://パーティクルコンテナの操作


private://パーティクル全体の操作
	//グラフィックスパイプライン
	void GenerateGraphicsPipeline();
private://エフェクトの操作
	

public://ゲッター
public://セッター
	void SetCamera(BaseCamera* camera) { camera_ = camera; }
private://インスタンス
	BaseCamera* camera_ = nullptr;
private://メンバ変数
	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	//グラフィックスパイプライン
	std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, (int)BlendMode::kMaxBlendModeNum> graphicsPipelineState;

	//パーティクルのコンテナ
	std::unordered_map<std::string, Particle*> particles;
	//パーティクルのモデルリソースのコンテナ
	std::unordered_map<std::string, Particle::ParticleResource> particleResources;

};

