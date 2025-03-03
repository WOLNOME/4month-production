#pragma once
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wrl.h>
#include <string>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class TextWriter {
public:

	void Initialize();
	void Update();
	void Draw();

private:
	ComPtr<ID3D11DeviceContext> m_d3d11DeviceContext;

};
