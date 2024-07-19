#include <Model.h>

#include <DxCommon.h>
#include <DxCommand.h>
#include <DxResourceCreator.h>

#include <ModelManager.h>
#include <CameraManager.h>


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

	transformBuffer_ = ONE::DxResourceCreate::CreateResource(sizeof(TransformData));
	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&transformData_));
	transformData_->matWorld = Mat4::kIdentity;

	viewProjectionBuffer_ = ONE::DxResourceCreate::CreateResource(sizeof(ViewProjectionData));
	viewProjectionBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&viewProjectionData_));
	viewProjectionData_->matVp = Mat4::kIdentity;

	transform_.position.z = 8.0f;
	transform_.Initialize();

}

void Model::Draw() {
	ModelManager::GetInstance()->SetPipelineState(kSolid);

	ID3D12GraphicsCommandList* commandList = ONE::DxCommon::GetInstance()->GetCommand()->GetList();

	transformData_->matWorld = transform_.matTransform;
	viewProjectionData_->matVp = CameraManager::GetInstance()->GetMainCamera()->GetMatProjection();

	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootConstantBufferView(0, transformBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, viewProjectionBuffer_->GetGPUVirtualAddress());

	commandList->DrawInstanced(3, 1, 0, 0);


}



void Model::SetFillMode(FillMode fillMode) {
	fillMode_ = fillMode;
}
