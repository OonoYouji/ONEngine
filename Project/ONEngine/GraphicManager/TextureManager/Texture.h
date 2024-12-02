#pragma once

/// directX
#include <d3d12.h>
#include <wrl/client.h>

/// std
#include <array>

/// engine
#include "Math/Vector2.h"

using namespace Microsoft::WRL;

/// ===================================================
/// テクスチャ 1枚あたりのクラス
/// ===================================================
class Texture final {
	friend class TextureManager;
public:

	enum DESCRIPTOR_INDEX_TYPE {
		SRV, UAV,
		COUNT
	};

	Texture();
	Texture(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);
	~Texture();

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const { return gpuHandle_; }

	Vec2 GetTextureSize() const;

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	uint32_t GetDescriptorIndex(DESCRIPTOR_INDEX_TYPE _type) const { return descriptorIndices_[_type]; }

	D3D12_RESOURCE_STATES GetCurrentState() const { return currentState_; }
	void SetCurrentState(D3D12_RESOURCE_STATES _state) { 
		currentState_ = _state;
	}

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12Resource> resource_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_{};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};

	std::array<uint32_t, COUNT> descriptorIndices_;

	D3D12_RESOURCE_STATES currentState_;
public:
	inline Texture& operator=(const Texture&) = default;
};