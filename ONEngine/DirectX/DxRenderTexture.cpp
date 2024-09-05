#include "DxRenderTexture.h"

#include <d3dx12.h>

#include <WinApp.h>
#include <DxCommon.h>
#include <DxDescriptor.h>
#include <DxResourceCreator.h>
#include <DxBarrierCreator.h>

#include <Matrix4x4.h>
#include <Vector2.h>

std::unique_ptr<PipelineState> DxRenderTexture::sPipeline_ = nullptr;
ComPtr<ID3D12Resource> DxRenderTexture::sViewProjectionBuffer_ = nullptr;
ComPtr<ID3D12Resource> DxRenderTexture::sVertexBuffer_ = nullptr;
D3D12_VERTEX_BUFFER_VIEW DxRenderTexture::sVbv_{};


void DxRenderTexture::StaticInitialize() {

	/// ===================================================
	/// pipeline state initialize
	/// ===================================================
	PipelineState::Shader shader{};
	shader.ShaderCompile(
		L"BlendRenderTexture/BlendRenderTexture.VS.hlsl", L"vs_6_0",
		L"BlendRenderTexture/BlendRenderTexture.PS.hlsl", L"ps_6_0"
	);

	sPipeline_.reset(new PipelineState);

	sPipeline_->SetShader(&shader);
	sPipeline_->SetFillMode(kSolid);
	sPipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	sPipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	sPipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	/// vs cbuffer
	sPipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);

	/// ps cbuffer
	sPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	sPipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	sPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	sPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);
	sPipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	sPipeline_->Initialize();


	/// ===================================================
	/// view projection buffer initialize
	/// ===================================================
	Mat4* viewProjectionData = nullptr;
	Matrix4x4 projectionMatrix = Mat4::MakeOrthographicMatrix(
		0.0f, 0.0f, float(ONE::WinApp::kWindowSizeX), float(ONE::WinApp::kWindowSizeY), 0.0f, 1000.0f);

	sViewProjectionBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4));
	sViewProjectionBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&viewProjectionData));
	*viewProjectionData = projectionMatrix;


	/// ===================================================
	/// vbv ivb
	/// ===================================================

	struct VertexPosUv {
		Vector4 position;
		Vector2 texcoord;
	};

	VertexPosUv* vertexPosUv = nullptr;
	sVertexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(VertexPosUv) * 3);
	sVertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexPosUv));
	vertexPosUv[0] = { Vec4(0,0,0,1), Vec2(0,0) };
	vertexPosUv[1] = { Vec4(2560,0,0,1), Vec2(2,0) };
	vertexPosUv[2] = { Vec4(0,1440,0,1), Vec2(0,2) };

	sVbv_.BufferLocation = sVertexBuffer_->GetGPUVirtualAddress();
	sVbv_.SizeInBytes = static_cast<UINT>(sizeof(VertexPosUv) * 3);
	sVbv_.StrideInBytes = static_cast<UINT>(sizeof(VertexPosUv));

}

void DxRenderTexture::StaticFinalize() {
	sVertexBuffer_.Reset();
	sViewProjectionBuffer_.Reset();
	sPipeline_.reset();
}



ComPtr<ID3D12Resource> DxRenderTexture::CreateRenderTextureResource(
	ID3D12Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor) {

	ComPtr<ID3D12Resource> resource;

	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		format,
		width,
		height,
		1, 1, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;

	device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clearValue,
		IID_PPV_ARGS(&resource)
	);

	return resource;
}



void DxRenderTexture::Initialize(const Vector4& clearColor, ID3D12GraphicsCommandList* commandList, ONE::DxDescriptor* descriptor) {

	ID3D12Device* device = ONE::DxCommon::GetInstance()->GetDevice();
	pCommandList_ = commandList;
	pDxDescriptor_ = descriptor;

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearColor_ = clearColor;
	renderTextureResource_ =
		CreateRenderTextureResource(device, 1280, 720, format, clearColor_);


	rtvHandle_.cpuHandle = pDxDescriptor_->GetRtvCpuHandle();
	pDxDescriptor_->AddRtvUsedCount();

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(renderTextureResource_.Get(), &rtvDesc, rtvHandle_.cpuHandle);


	srvHandle_.cpuHandle = pDxDescriptor_->GetSrvCpuHandle();
	srvHandle_.gpuHandle = pDxDescriptor_->GetSrvGpuHandle();
	pDxDescriptor_->AddSrvUsedCount();

	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	renderTextureSrvDesc.Format = format;
	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(renderTextureResource_.Get(), &renderTextureSrvDesc, srvHandle_.cpuHandle);
}

void DxRenderTexture::SetRenderTarget() {
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = pDxDescriptor_->GetDsvCpuHandle();
	pCommandList_->OMSetRenderTargets(1, &rtvHandle_.cpuHandle, FALSE, &dsvHandle);
	pCommandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	pCommandList_->ClearRenderTargetView(rtvHandle_.cpuHandle, &clearColor_.x, 0, nullptr);
}



void DxRenderTexture::BlendRenderTexture(DxRenderTexture* frontRenderTex, DxRenderTexture* output) {
	ONE::DxBarrierCreator::CreateBarrier(frontRenderTex->GetRenderTexResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	ONE::DxBarrierCreator::CreateBarrier(this->GetRenderTexResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	
	output->SetRenderTarget();
	sPipeline_->SetPipelineState();

	pCommandList_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList_->IASetVertexBuffers(0, 1, &sVbv_);

	/// view projection
	pCommandList_->SetGraphicsRootConstantBufferView(0, sViewProjectionBuffer_->GetGPUVirtualAddress());

	/// render tex
	pCommandList_->SetGraphicsRootDescriptorTable(1, this->srvHandle_.gpuHandle);
	pCommandList_->SetGraphicsRootDescriptorTable(2, frontRenderTex->srvHandle_.gpuHandle);

	pCommandList_->DrawInstanced(3, 1, 0, 0);

	ONE::DxBarrierCreator::CreateBarrier(frontRenderTex->GetRenderTexResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	ONE::DxBarrierCreator::CreateBarrier(this->GetRenderTexResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
}
