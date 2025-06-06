#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <list>
#include <cstdint>

class SrvManager
{
private:
	static SrvManager* instance;

	SrvManager() = default;
	~SrvManager() = default;
	SrvManager(SrvManager&) = delete;
	SrvManager& operator=(SrvManager&) = delete;

public:
	static SrvManager* GetInstance();

	// 初期化
	void Initialize();
	// 終了
	void Finalize();

	// Debug用
	void DebugWithImGui();

	// 描画前設定
	void PreDraw(ID3D12GraphicsCommandList* pCommandList);
	// 割り当て用関数
	uint32_t Allocate();
	// 解放用関数
	void Free(uint32_t srvIndex);
	// 空きインデックスの存在確認用関数
	bool CheckCanSecured();

	// SRV生成関数
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	// ゲッター
	ID3D12DescriptorHeap* GetDescriptorHeap() const { return descriptorHeap.Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, uint32_t srvIndex);

	static const uint32_t kMaxSRVCount;

private:
	//デスクリプタのサイズ
	uint32_t descriptorSize = 0;
	//デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	//最新の空きインデックス
	uint32_t useIndex = 0;

	// 空きインデックスを管理するキュー(Free関数によって割り当てられる)
	std::list<uint32_t> freeIndices;
};
