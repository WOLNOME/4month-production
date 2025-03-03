#include "TextWriter.h"
#include "DirectXCommon.h"
#include <d3d11on12.h>

void TextWriter::Initialize() {
	auto dxcommon = DirectXCommon::GetInstance();

	//IDWriteFactoryの生成
	ComPtr<IDWriteFactory> directWriteFactory;
	if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &directWriteFactory)))[[unlikely]] {
		return;
	}
	//ID3D11On12Deviceの生成
	ComPtr<ID3D11Device
	> d3d11Device = nullptr;
	UINT d3d11DeviceFlags = 0U;
#ifdef _DEBUG
	d3d11DeviceFlags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#else
	d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif // _DEBUG

	if (FAILED(D3D11On12CreateDevice(dxcommon->GetDevice(), d3d11DeviceFlags, nullptr, 0U, reinterpret_cast<IUnknown**>(dxcommon->GetCommandQueue()), 1U, 0U, &d3d11Device, &m_d3d11DeviceContext, nullptr))) [[unlikely]] {
		return;
	}

	if(SUCCEEDED(d3d))



}

void TextWriter::Update() {
}

void TextWriter::Draw() {
}
