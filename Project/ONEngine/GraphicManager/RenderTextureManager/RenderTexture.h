#pragma once

#include <d3d12.h>
#include <wrl/client.h>

/// std
#include <cmath>
#include <memory>

/// engine
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptorHeap.h"
#include "GraphicManager/PipelineState/PipelineState.h"

/// math
#include "Math/Vector4.h"

using namespace Microsoft::WRL;


class RenderTexture {
public:

	RenderTexture() {}
	~RenderTexture() {}

	static ComPtr<ID3D12Resource> CreateRenderTextureResource(
		ID3D12Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor
	);


	void Initialize(
		const Vector4& clearColor,
		ID3D12GraphicsCommandList* commandList,
		ONE::DxDescriptorHeap<ONE::HeapType::CBV_SRV_UAV>* _srvDescriptorHeap,
		ONE::DxDescriptorHeap<ONE::HeapType::RTV>*         _rtvDescriptorHeap,
		ONE::DxDescriptorHeap<ONE::HeapType::DSV>*         _dsvDescriptorHeap
	);

	void SetRenderTarget();

	void BeginRenderTarget();
	void EndRenderTarget();

	void ClearDepth();

	void BlendRenderTexture(RenderTexture* frontRenderTex, RenderTexture* output);

	ID3D12Resource* GetRenderTexResource() const {
		return renderTextureResource_.Get();
	}

	
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHandle() {
		return srvHandle_.gpuHandle;
	}
	
	D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHandle() {
		return srvHandle_.cpuHandle;
	}


	D3D12_RESOURCE_STATES currentResourceState;


private:

	struct DescriptorHandle final {
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	};

	Vec4 clearColor_;
	DescriptorHandle rtvHandle_;
	DescriptorHandle srvHandle_;

	ComPtr<ID3D12Resource> renderTextureResource_;

	/// other class pointer
	ID3D12GraphicsCommandList* pCommandList_;
	ONE::DxDescriptorHeap<ONE::HeapType::DSV>* pDSVDescriptorHeap_ = nullptr;

};