#include <Model.h>

#include <DxCommon.h>
#include <DxCommand.h>
#include <DxResourceCreator.h>


Model::Model() {}
Model::~Model() {}



void Model::Initialize() {

	vertexBuffer_ = ONE::DxResourceCreate::CreateResource(sizeof(VertexData) * 3);

	vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbv_.SizeInBytes = sizeof(VertexData) * 3;
	vbv_.StrideInBytes = sizeof(VertexData);

	VertexData* vertexData_ = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	vertexData_[0] = { {-0.5f, -0.25f, 0.0f, 1.0f} };
	vertexData_[1] = { { 0.0f,  0.25f, 0.0f, 1.0f} };
	vertexData_[2] = { { 0.5f, -0.25f, 0.0f, 1.0f} };


}

void Model::Draw() {
	ModelManager::GetInstance()->SetPipelineState(kSolid);

	ID3D12GraphicsCommandList* commandList = ONE::DxCommon::GetInstance()->GetCommand()->GetList();

	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->DrawInstanced(3, 1, 0, 0);


}



void Model::SetFillMode(FillMode fillMode) {
	fillMode_ = fillMode;
}
