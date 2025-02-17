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

};

