#pragma once

#include <d3d12.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

/// ===================================================
/// テクスチャ 1枚あたりのクラス
/// ===================================================
class Texture final {
	friend class TextureManager;
public:

	Texture() = default;
	~Texture() = default;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const { return gpuHandle_; }

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12Resource> resource_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_{};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};

public:
	inline Texture& operator=(const Texture&) = default;
};