#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cmath>
#include <memory>

#include <PipelineState.h>
#include <Vector4.h>

using namespace Microsoft::WRL;

namespace ONE {
	class DxDescriptor;
}

class DxRenderTexture {
public:

	DxRenderTexture() {}
	~DxRenderTexture() {}

	static void StaticInitialize();
	static void StaticFinalize();

	static ComPtr<ID3D12Resource> CreateRenderTextureResource(
		ID3D12Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor
	);


	void Initialize(
		const Vector4& clearColor,
		ID3D12GraphicsCommandList* commandList,
		ONE::DxDescriptor* descriptor
	);

	void SetRenderTarget();

	void BlendRenderTexture(DxRenderTexture* frontRenderTex, DxRenderTexture* output);

	ID3D12Resource* GetRenderTexResource() const {
		return renderTextureResource_.Get();
	}

private:

	static std::unique_ptr<PipelineState> sPipeline_;
	static ComPtr<ID3D12Resource> sViewProjectionBuffer_;
	static ComPtr<ID3D12Resource> sVertexBuffer_;
	static D3D12_VERTEX_BUFFER_VIEW sVbv_;

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
	ONE::DxDescriptor* pDxDescriptor_;
};