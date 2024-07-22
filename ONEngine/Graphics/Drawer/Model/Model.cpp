#include <Model.h>

#include <DxCommon.h>
#include <DxCommand.h>
#include <DxResourceCreator.h>

#include <ModelManager.h>
#include <CameraManager.h>
#include <TextureManager.h>



Model::Model() {
	//ModelManager::GetInstance()->AddModel(this);
}
Model::~Model() {
	meshes_.clear();
	materials_.clear();
}



void Model::Initialize() {

	transformBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(TransformData));
	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&transformData_));
	transformData_->matWorld = Mat4::kIdentity;

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

	commandList->SetGraphicsRootConstantBufferView(1, transformBuffer_->GetGPUVirtualAddress());


	for(uint32_t index = 0; index < meshes_.size(); ++index) {
		materials_[index].BindMaterial(commandList, 2);
		materials_[index].BindTexture(commandList, 3);
		meshes_[index].Draw(commandList, true);
	}
	//commandList->DrawInstanced(3, 1, 0, 0);
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


void Model::SetRotate(const Vec3& rotate) {
	transform_.rotate = rotate;
	transform_.UpdateMatrix();
}