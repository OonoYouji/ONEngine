#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "Math/Vector2.h"

using namespace Microsoft::WRL;

/// ===================================================
/// テクスチャ 1枚あたりのクラス
/// ===================================================
class Texture final {
	friend class TextureManager;
public:

	Texture() = default;
	Texture(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);
	~Texture() = default;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const { return gpuHandle_; }

	Vec2 GetTextureSize() const;

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