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
	ModelManager::GetInstance()->AddActiveModel(this, transform, nullptr, fillMode);
}

void Model::Draw(Transform* transform, Material* material, FillMode fillMode) {
	ModelManager::GetInstance()->AddActiveModel(this, transform, material, fillMode);
}


/// ===================================================
/// DrawCallの呼び出し
/// ===================================================
void Model::DrawCall(ID3D12GraphicsCommandList* commandList, Material* material, uint32_t materialRootParamIndex, uint32_t textureRootParamIndex) {
	if(material) {
		for(uint32_t index = 0; index < materials_.size(); ++index) {
			material->BindMaterial(commandList, 2);
			material->BindTexture(commandList, 4);
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

