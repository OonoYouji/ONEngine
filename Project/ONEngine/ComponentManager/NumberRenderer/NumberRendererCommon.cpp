#include "NumberRendererCommon.h"

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"


void NumberRendererCommon::Initialize() {

	/// ---------------------------------------------------
	/// psoの初期化
	/// ---------------------------------------------------

	/// shader compile
	shader_.ShaderCompile(
		L"NumberRender/NumberRender.VS.hlsl", L"vs_6_0",
		L"NumberRender/NumberRender.PS.hlsl", L"ps_6_0"
	);

	pipelineState_.reset(new PipelineState);

	/// default setting
	pipelineState_->SetShader(&shader_);
	pipelineState_->SetFillMode(kSolid);
	pipelineState_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);


	/// input layout setting
	pipelineState_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipelineState_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);


	/// buffer setting
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// view projection
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);  /// material

	pipelineState_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	pipelineState_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// transform 
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);  /// texture
	pipelineState_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	pipelineState_->Initialize();



	/// ---------------------------------------------------
	/// bufferの作成
	/// ---------------------------------------------------

	/// vertices
	vertices_.push_back({ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f } });
	vertices_.push_back({ {  0.5f, -0.5f, 0.0f, 1.0f }, { 1.0f, 0.0f } });
	vertices_.push_back({ { -0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 1.0f } });
	vertices_.push_back({ {  0.5f,  0.5f, 0.0f, 1.0f }, { 1.0f, 1.0f } });

	/// indices
	indices_.push_back(0);
	indices_.push_back(1);
	indices_.push_back(2);

	indices_.push_back(2);
	indices_.push_back(1);
	indices_.push_back(3);


	/// vertex buffer create
	size_t vertexDataSize = sizeof(NumberRendererVertexData);
	vertexBuffer_ = ONE::DxResourceCreator::CreateResource(vertexDataSize * vertices_.size());

	NumberRendererVertexData* vertexData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), vertexDataSize * vertices_.size());

	vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbv_.SizeInBytes    = static_cast<UINT>(vertexDataSize * vertices_.size());
	vbv_.StrideInBytes  = static_cast<UINT>(vertexDataSize);


	/// index buffer create
	indexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(uint32_t) * indices_.size());

	uint32_t* indexData = nullptr;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());

	ibv_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibv_.Format         = DXGI_FORMAT_R32_UINT;
	ibv_.SizeInBytes    = static_cast<UINT>(sizeof(uint32_t) * indices_.size());


}

void NumberRendererCommon::Finalize() {
	pipelineState_.reset();
}



void NumberRendererCommon::PreDraw() {

}

void NumberRendererCommon::PostDraw() {

	ID3D12GraphicsCommandList* pCommandList = ONEngine::GetDxCommon()->GetDxCommand()->GetList();

	pipelineState_->SetPipelineState();

	/// default setting

	/// vbv, ivb setting
	pCommandList->IASetIndexBuffer(&ibv_);
	pCommandList->IASetVertexBuffers(0, 1, &vbv_);



}
