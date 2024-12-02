#include "Texture.h"

#include "Core/ONEngine.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptorHeap.h"

Texture::Texture() {
	descriptorIndices_.fill(-1);
}

Texture::Texture(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) {
	cpuHandle_ = cpuHandle;
	gpuHandle_ = gpuHandle;
	descriptorIndices_.fill(-1);
}

Texture::~Texture() {
	/*auto srvHeap = ONEngine::GetDxCommon()->GetSRVDescriptorHeap();

	for(size_t i = 0; i < descriptorIndices_.size(); ++i) {
		srvHeap->Free(descriptorIndices_[i]);
	}*/
}

Vec2 Texture::GetTextureSize() const {
	if(!resource_) {
		return Vec2(128, 128);
	}
	D3D12_RESOURCE_DESC desc = resource_->GetDesc();

	return Vec2(
		static_cast<float>(desc.Width),
		static_cast<float>(desc.Height)
	);
}
