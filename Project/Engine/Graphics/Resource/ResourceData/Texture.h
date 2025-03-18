#pragma once

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"

/// ///////////////////////////////////////////////////
/// texture
/// ///////////////////////////////////////////////////
class Texture final {
	friend class GraphicsResourceLoader;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Texture();
	~Texture();


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	DxResource                  dxResource_;

	uint32_t                    srvDescriptorIndex_;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief Set the CPU descriptor handle
	/// @param _cpuHandle The CPU descriptor handle to set
	void SetCPUDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle) { cpuHandle_ = _cpuHandle; }

	/// @brief Set the GPU descriptor handle
	/// @param _gpuHandle The GPU descriptor handle to set
	void SetGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle) { gpuHandle_ = _gpuHandle; }

	/// @brief Set the SRV heap index
	/// @param _index The SRV heap index to set
	void SetSRVHeapIndex(uint32_t _index) { srvDescriptorIndex_ = _index; }



	/// @brief Get the CPU descriptor handle
	/// @return The CPU descriptor handle
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle() const { return cpuHandle_; }

	/// @brief Get the GPU descriptor handle
	/// @return The GPU descriptor handle
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle() const { return gpuHandle_; }

	/// @brief Get the SRV heap index
	/// @return The SRV heap index
	uint32_t GetSRVHeapIndex() const { return srvDescriptorIndex_; }

	/// @brief resourceを取得
	/// @return DxResource
	const DxResource& GetDxResource() const { return dxResource_; }

	/// @brief resourceを取得
	/// @return DxResource
	DxResource& GetDxResource() { return dxResource_; }
};
