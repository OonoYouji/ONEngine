#include <Texture.h>

Texture::Texture(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) {
	cpuHandle_ = cpuHandle;
	gpuHandle_ = gpuHandle;
}

Vec2 Texture::GetTextureSize() const {
	D3D12_RESOURCE_DESC desc = resource_->GetDesc();

	return Vec2(
		static_cast<float>(desc.Width),
		static_cast<float>(desc.Height)
	);
}
