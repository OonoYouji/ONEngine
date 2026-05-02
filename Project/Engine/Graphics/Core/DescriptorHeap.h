#pragma once

/// directX
#include <d3d12.h>

/// std
#include <cstdint>
#include <vector>

/// engine::graphics
#include "Engine/Graphics/Utils/ComPtr.h"

namespace Engine::Graphics {
class RenderDevice;
}

namespace Engine::Graphics {

///
/// DX12のデスクリプタヒープを管理するクラス
///
class DescriptorHeap final {
public:

	DescriptorHeap();
	~DescriptorHeap();

	void Initialize(RenderDevice* renderDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, bool shaderVisible);
	void Shutdown();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index) const;

	ID3D12DescriptorHeap* GetHeap() const { return descriptorHeap_.Get(); }

private:

	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	uint32_t descriptorSize_;
	D3D12_CPU_DESCRIPTOR_HANDLE startCPUHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE startGPUHandle_;

};

} /// namespace Engine::Graphics