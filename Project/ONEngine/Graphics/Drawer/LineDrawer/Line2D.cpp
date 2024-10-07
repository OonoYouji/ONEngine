#include "Line2D.h"

#include <cassert>

#include <Core/ONEngine.h>

#include <DxCommand.h>
#include <DxResourceCreator.h>
#include <WinApp.h>



const int Line2D::kMaxInstanceCount_ = 1024;

Line2D* Line2D::GetInstance() {
	static Line2D instance;
	return &instance;
}

void Line2D::Initialize() {

	pipeline_.reset(new PipelineState());
	shader_.ShaderCompile(
		L"Line2D/Line2D.VS.hlsl", L"vs_6_0",
		L"Line2D/Line2D.PS.hlsl", L"ps_6_0"
	);
	shader_.GeometryShaderCompile(
		L"Line2D/Line2D.GS.hlsl", L"gs_6_0"
	);

	pipeline_->SetFillMode(kSolid);

	pipeline_->SetShader(&shader_);

	pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

	pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipeline_->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	pipeline_->Initialize();

	CreateVertexBuffer(kMaxInstanceCount_);
}

void Line2D::Finalize() {
	vertices_.clear();
	pipeline_.reset();
	vertexBuffer_.Reset();
}

void Line2D::PreDraw() {
	vertices_.clear();
}

void Line2D::PostDraw() {

	assert(vertices_.size() < kMaxInstanceCount_);
	
	if(vertices_.empty()) { return; }

	LineVertexDate* vertexDate = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate));
	std::memcpy(vertexDate, vertices_.data(), sizeof(LineVertexDate) * vertices_.size());


	ID3D12GraphicsCommandList* cList = ONEngine::GetDxCommon()->GetDxCommand()->GetList();
	pipeline_->SetPipelineState();

	cList->IASetVertexBuffers(0, 1, &vbv_);
	cList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	cList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);
}

void Line2D::Draw(const Vector2& v1, const Vector2& v2, const Vector4& color) {

	vertices_.push_back(LineVertexDate(Vector4(v1.x, v1.y, 0.0f, 1.0f), color));
	vertices_.push_back(LineVertexDate(Vector4(v2.x, v2.y, 0.0f, 1.0f), color));

}

void Line2D::CreateVertexBuffer(size_t vertexCount) {

	vertexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(LineVertexDate) * vertexCount);

	vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbv_.SizeInBytes = static_cast<UINT>(sizeof(LineVertexDate) * vertexCount);
	vbv_.StrideInBytes = static_cast<UINT>(sizeof(LineVertexDate));

	
}
