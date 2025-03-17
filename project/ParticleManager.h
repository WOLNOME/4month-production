#pragma once
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

class Particle;

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
public://パーティクルコンテナの操作


private://非公開メンバ関数
	//グラフィックスパイプライン
	void GenerateGraphicsPipeline();

public://ゲッター
public://セッター
private://インスタンス

private://メンバ変数
	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	//グラフィックスパイプライン
	std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, (size_t)BlendMode::kMaxBlendModeNum> graphicsPipelineState;

	//パーティクルのコンテナ
	std::unordered_map<std::string, std::unique_ptr<Particle>> particles;

};

