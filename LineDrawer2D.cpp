#include "LineDrawer2D.h"
#include <DxCommand.h>
#include <DxResourceCreator.h>
#include <WinApp.h>


LineDrawer2D* LineDrawer2D::GetInstance() {
	static LineDrawer2D instance;
	return &instance;
}

void LineDrawer2D::Initialize() {

	pipeline_.reset(new PipelineState());
	shader_.ShaderCompile(
		L"Line2D.VS.hlsl", L"vs_6_0",
		L"Line2D.PS.hlsl", L"ps_6_0"
	);

	pipeline_->SetFillMode(kSolid);

	pipeline_->SetShader(&shader_);

	pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

	pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipeline_->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	pipeline_->Initialize();

}

void LineDrawer2D::Finalize() {
	vertices_.clear();
	pipeline_.reset();
	vertexBuffer_.Reset();
}

void LineDrawer2D::PreDraw() {
	vertices_.clear();
}

void LineDrawer2D::PostDraw() {

	if (vertices_.empty()) { return; }
	ID3D12GraphicsCommandList* cList = ONE::DxCommon::GetInstance()->GetDxCommand()->GetList();
	pipeline_->SetPipelineState();

	CreateVertexBuffer(vertices_.size());

	cList->IASetVertexBuffers(0, 1, &vbv_);
	cList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	cList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);
}

void LineDrawer2D::Draw(const Vector2& v1, const Vector2& v2, const Vector4& color) {

	vertices_.push_back({ { v1.x, v1.y, 0.0f, 1.0f },color,{ONE::WinApp::kWindowSizeX,ONE::WinApp::kWindowSizeY,0.0f,0.0f} });
	vertices_.push_back({ { v2.x, v2.y, 0.0f, 1.0f },color,{ONE::WinApp::kWindowSizeX,ONE::WinApp::kWindowSizeY,0.0f,0.0f} });

}

void LineDrawer2D::CreateVertexBuffer(size_t vertexCount) {

	vertexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(LineVertexDate) * vertexCount);

	vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbv_.SizeInBytes = static_cast<UINT>(sizeof(LineVertexDate) * vertexCount);
	vbv_.StrideInBytes = static_cast<UINT>(sizeof(LineVertexDate));

	LineVertexDate* vertexDate = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate));
	std::memcpy(vertexDate, vertices_.data(), sizeof(LineVertexDate) * vertexCount);

}
