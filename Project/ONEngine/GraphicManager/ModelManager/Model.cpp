#include "Model.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"

#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/TextureManager/TextureManager.h"
#include "Objects/Camera/Manager/CameraManager.h"



Model::Model() {}
Model::~Model() {}


/// ===================================================
/// 初期化処理
/// ===================================================
void Model::Initialize() {

}


/// ===================================================
/// 描画
/// ===================================================
void Model::Draw(Transform* transform, FillMode fillMode) {
	ModelManager::GetInstance()->AddActiveModel(this, transform, nullptr, nullptr, fillMode);
}

void Model::Draw(Transform* transform, Mat4* matLocal, Material* material, FillMode fillMode) {
	ModelManager::GetInstance()->AddActiveModel(this, transform, matLocal, material, fillMode);
}


/// ===================================================
/// DrawCallの呼び出し
/// ===================================================
void Model::DrawCall(ID3D12GraphicsCommandList* commandList, Material* material, uint32_t materialRootParamIndex, uint32_t textureRootParamIndex) {
	if(material) {
		for(uint32_t index = 0; index < materials_.size(); ++index) {
			material->BindMaterial(commandList, materialRootParamIndex);
			material->BindTexture(commandList, textureRootParamIndex);
		}
	} else {
		for(uint32_t index = 0; index < materials_.size(); ++index) {
			materials_[index].BindMaterial(commandList, materialRootParamIndex);
			materials_[index].BindTexture(commandList, textureRootParamIndex);
		}
	}

	for(uint32_t index = 0; index < meshes_.size(); ++index) {
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

void Model::SetRootNode(const Node& _root) {
	root_ = _root;
}

