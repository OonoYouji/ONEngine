#include <Texture.h>

Texture::Texture(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) {
	cpuHandle_ = cpuHandle;
	gpuHandle_ = gpuHandle;
}
