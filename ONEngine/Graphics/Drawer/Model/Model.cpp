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
	/*meshes_.clear();
	materials_.clear();*/
}


/// ===================================================
/// 初期化処理
/// ===================================================
void Model::Initialize() {

}


/// ===================================================
/// 描画
/// ===================================================
void Model::Draw(Transform* transform, FillMode fillMode) {
	ModelManager::GetInstance()->AddActiveModel(this, transform, fillMode);
}


/// ===================================================
/// DrawCallの呼び出し
/// ===================================================
void Model::DrawCall(ID3D12GraphicsCommandList* commandList) {

	for(uint32_t index = 0; index < meshes_.size(); ++index) {
		materials_[index].BindMaterial(commandList, 2);
		materials_[index].BindTexture(commandList, 3);
		meshes_[index].Draw(commandList, true);
	}

}


/// ===================================================
/// FillModeのセット
/// ===================================================
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

