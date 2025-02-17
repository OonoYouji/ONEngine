#pragma once

/// engine
#include "Engine/Core/DirectX12/Resource/DxResource.h"


/// ===================================================
/// texture
/// ===================================================
class Texture final {
	friend class GraphicsResourceLoader;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Texture();
	~Texture();

public:
	
	/// ===================================================
	/// public : objects
	/// ===================================================

	DxResource                  dxResource_;

	uint32_t                    srvDescriptorIndex_;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================


	/// @brief cpu handleの取得
	/// @return cpu handle
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle() const { return cpuHandle_; }

	/// @brief gpu handleの取得
	/// @return gpu handle
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle() const { return gpuHandle_; }

};

