#include <Model.h>

#include <DxCommon.h>
#include <DxCommand.h>
#include <DxResourceCreator.h>

#include <ModelManager.h>
#include <CameraManager.h>
#include <TextureManager.h>



Model::Model() {
	ModelManager::GetInstance()->AddModel(this);
}
Model::~Model() {}



void Model::Initialize() {

	vertexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(VertexData) * 3);

	vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbv_.SizeInBytes = sizeof(VertexData) * 3;
	vbv_.StrideInBytes = sizeof(VertexData);

	VertexData* vertexData_ = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	vertexData_[0] = { {-0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f} };
	vertexData_[1] = { { 0.0f,  0.5f, 0.0f, 1.0f}, {0.5f, 0.0f} };
	vertexData_[2] = { { 0.5f, -0.5f, 0.0f, 1.0f}, {1.0f, 1.0f} };

	transformBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(TransformData));
	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&transformData_));
	transformData_->matWorld = Mat4::kIdentity;

	materialBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(MaterialData));
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vec4(1,0,0,1);

	transform_.position.z = 8.0f;
	transform_.Initialize();

}

void Model::Draw() {
	/*ModelManager::GetInstance()->SetPipelineState(kSolid);

	ID3D12GraphicsCommandList* commandList = ONE::DxCommon::GetInstance()->GetCommand()->GetList();

	transformData_->matWorld = transform_.matTransform;

	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootConstantBufferView(0, transformBuffer_->GetGPUVirtualAddress());

	commandList->DrawInstanced(3, 1, 0, 0);*/


}

void Model::DrawCall(ID3D12GraphicsCommandList* commandList) {
	transformData_->matWorld = transform_.matTransform;

	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->SetGraphicsRootConstantBufferView(1, transformBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(2, materialBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(3, TextureManager::GetInstance()->GetTexture("uvChecker.png").GetGPUHandle());

	commandList->DrawInstanced(3, 1, 0, 0);
}



void Model::SetFillMode(FillMode fillMode) {
	fillMode_ = fillMode;
}


/// ===================================================
/// Meshの追加
/// ===================================================
void Model::AddMesh(const Mesh& mesh) {
	meshes_.push_back(mesh);
}


/// ===================================================
/// Materialの追加
/// ===================================================
void Model::AddMaterial(const Material& material) {
	materials_.push_back(material);
}
