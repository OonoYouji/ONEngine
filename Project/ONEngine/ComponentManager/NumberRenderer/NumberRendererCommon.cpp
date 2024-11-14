#include "NumberRendererCommon.h"

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"

#include "Objects/Camera/Manager/CameraManager.h"


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
	pipelineState_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// transform 
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);  /// texture
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 2);  /// number digit
	pipelineState_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	pipelineState_->Initialize();



	/// ---------------------------------------------------
	/// bufferの作成
	/// ---------------------------------------------------
	
	/// vertices
	vertices_.push_back({ { -0.5f, -0.5f, 0.0f, 1.0f}, { 0.0f, 1.0f } }); /// 左下
	vertices_.push_back({ { -0.5f,  0.5f, 0.0f, 1.0f}, { 0.0f, 0.0f } }); /// 左上
	vertices_.push_back({ {  0.5f,  0.5f, 0.0f, 1.0f}, { 1.0f, 0.0f } }); /// 右上
	vertices_.push_back({ {  0.5f, -0.5f, 0.0f, 1.0f}, { 1.0f, 1.0f } }); /// 右下

	/// indices
	indices_.push_back(0);
	indices_.push_back(1);
	indices_.push_back(2);

	indices_.push_back(0);
	indices_.push_back(2);
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

	vertexBuffer_.Reset();
	indexBuffer_.Reset();
}



void NumberRendererCommon::PreDraw() {
	activeList_.clear();
}

void NumberRendererCommon::PostDraw() {

	ID3D12GraphicsCommandList* pCommandList         = ONEngine::GetDxCommon()->GetDxCommand()->GetList();
	CameraManager*             cameraManager        = CameraManager::GetInstance();
	ID3D12Resource*            viewProjectionBuffer = cameraManager->GetMainCamera()->GetViewBuffer();

	pipelineState_->SetPipelineState();

	/// default setting
	pCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList->IASetIndexBuffer(&ibv_);
	pCommandList->IASetVertexBuffers(0, 1, &vbv_);

	/// buffer setting
	pCommandList->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer->GetGPUVirtualAddress());

	for(auto& numberRenderer : activeList_) {
		numberRenderer->DrawCall(pCommandList);
	}
}

void NumberRendererCommon::AddActive(NumberRenderer* _numberRenderer) {
	activeList_.push_back(_numberRenderer);
}
