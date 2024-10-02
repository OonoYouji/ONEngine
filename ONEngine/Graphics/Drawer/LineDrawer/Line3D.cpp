#include "Line3D.h"

#include <cassert>


void Line3D::SInitialize(ID3D12GraphicsCommandList* commandList) {
	Line3D* instance = GetInstance();

	instance->pCommandList_ = commandList;
	assert(instance->pCommandList_);


	instance->shader_.ShaderCompile(
		L"Line3D/Line3D.VS.hlsl", L"vs_6_0",
		L"Line3D/Line3D.PS.hlsl", L"ps_6_0"
	);

	instance->pipelineState_.reset(new PipelineState);

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


	pCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

}
