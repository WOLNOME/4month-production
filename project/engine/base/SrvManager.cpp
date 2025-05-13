#include "SrvManager.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include <cassert>

SrvManager* SrvManager::instance = nullptr;
const uint32_t SrvManager::kMaxSRVCount = 512;

SrvManager* SrvManager::GetInstance() {
	if (instance == nullptr) {
		instance = new SrvManager;
	}
	return instance;
}

void SrvManager::Initialize() {
	descriptorHeap = DirectXCommon::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount, true);
	descriptorSize = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void SrvManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void SrvManager::DebugWithImGui() {
#ifdef _DEBUG
	ImGui::Begin("SRVManager");
	ImGui::Text("空きの最新インデックス");
	ImGui::Text("%d", useIndex);
	ImGui::Text("キューにある空きインデックス");
	int count = 0;
	for (uint32_t i : freeIndices) {
		ImGui::Text("%d", i);
		count++;

		// 10個ごとに改行、それ以外は横に並べる
		if (count % 10 != 0) {
			ImGui::SameLine();
		}
	}
	ImGui::End();
#endif // _DEBUG

}

void SrvManager::PreDraw(ID3D12GraphicsCommandList* pCommandList) {
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap.Get() };
	pCommandList->SetDescriptorHeaps(1, descriptorHeaps);
}

uint32_t SrvManager::Allocate() {
	// 空きインデックスがあれば再利用
	if (!freeIndices.empty()) {
		uint32_t index = freeIndices.front();
		freeIndices.pop_front();
		return index;
	}

	// 上限に達していないかチェック
	if (useIndex >= kMaxSRVCount) {
		assert(0 && "SRVデスクリプタヒープがいっぱいです！");
		return UINT32_MAX; // エラーの場合
	}

	// 新しいインデックスを割り当てる
	return useIndex++;
}

void SrvManager::Free(uint32_t srvIndex) {
	// インデックスが範囲内であることを確認
	if (srvIndex < kMaxSRVCount) {
		freeIndices.push_back(srvIndex);
	}
}

bool SrvManager::CheckCanSecured() {
	return (useIndex < kMaxSRVCount || !freeIndices.empty());
}

void SrvManager::CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels) {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = MipLevels;

	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUDescriptorHandle(uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUDescriptorHandle(uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

void SrvManager::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, uint32_t srvIndex) {
	pCommandList->SetGraphicsRootDescriptorTable(RootParameterIndex, GetGPUDescriptorHandle(srvIndex));
}
