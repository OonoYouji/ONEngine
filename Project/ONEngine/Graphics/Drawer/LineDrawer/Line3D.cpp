#include "Line3D.h"

#include <cassert>

#include <DxResourceCreator.h>

#include <CameraManager.h>


void Line3D::SInitialize(ID3D12GraphicsCommandList* commandList) {
	Line3D* instance = GetInstance();

	/// command list pointer get
	instance->pCommandList_ = commandList;
	assert(instance->pCommandList_);


	/// shader compile
	instance->shader_.ShaderCompile(
		L"Line3D/Line3D.VS.hlsl", L"vs_6_0",
		L"Line3D/Line3D.PS.hlsl", L"ps_6_0"
	);

	/// pipeline create
	instance->pipelineState_.reset(new PipelineState);
	
	/// shader, fill mode
	instance->pipelineState_->SetShader(&instance->shader_);
	instance->pipelineState_->SetFillMode(kSolid);
	/// topology
	instance->pipelineState_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	/// input layout
	instance->pipelineState_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	instance->pipelineState_->AddInputElement("COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	/// constant buffer
	instance->pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);
	/// create
	instance->pipelineState_->Initialize();



	/// vertex buffer 
	instance->vertexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(VertexData) * instance->kMaxLineNum_);

	/// vertex buffer view
	instance->vbv_.BufferLocation = instance->vertexBuffer_->GetGPUVirtualAddress();
	instance->vbv_.SizeInBytes    = static_cast<UINT>(sizeof(VertexData) * instance->kMaxLineNum_);
	instance->vbv_.StrideInBytes  = static_cast<UINT>(sizeof(VertexData));

}

void Line3D::SFinalize() {
	Line3D* instance = GetInstance();
	instance->vertexBuffer_.Reset();
	instance->pipelineState_.reset();
	instance->vertices_.clear();
}


void Line3D::Draw(const Vec3& v1, const Vec3& v2, const Vec4& v1Color, const Vec4& v2Color) {
	vertices_.push_back(VertexData(Vec4(v1.x, v1.y, v1.z, 1.0f), v1Color));
	vertices_.push_back(VertexData(Vec4(v2.x, v2.y, v2.z, 1.0f), v2Color));
}


void Line3D::PreDraw() {
	vertices_.clear();
}

void Line3D::PostDraw() {
	if(vertices_.empty()) {
		return;
	}

	/// copy vertices
	VertexData* forwardPoint = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&forwardPoint));
	std::memcpy(forwardPoint, vertices_.data(), sizeof(VertexData) * vertices_.size());


	/// other pointer get
	ID3D12Resource* viewBuffer = CameraManager::GetInstance()->GetMainCamera()->GetViewBuffer();

	/// default setting
	pipelineState_->SetPipelineState();

	/// command setting
	pCommandList_->IASetVertexBuffers(0, 1, &vbv_);
	pCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	pCommandList_->SetGraphicsRootConstantBufferView(0, viewBuffer->GetGPUVirtualAddress());

	/// draw call
	pCommandList_->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);
}
