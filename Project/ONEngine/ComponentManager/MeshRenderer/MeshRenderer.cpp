#include "MeshRenderer.h"

#include "GraphicManager/ModelManager/ModelManager.h"
#include "GameObjectManager/BaseGameObject.h"


void MeshRenderer::Initialize() {
	pTransform_ = GetOwner()->GetComponent<Transform>();
}

void MeshRenderer::Draw() {
	model_->Draw(pTransform_, nullptr, material_.get(), fillMode_);
}



void MeshRenderer::SetModel(const std::string& filePath) {
	model_ = ModelManager::Load(filePath);
}

void MeshRenderer::SetModel(Model* model) {
	model_ = model;
}

void MeshRenderer::SetMaterial(const std::string& texName) {
	if(!material_) {
		material_.reset(new Material);
	}

	material_->CreateMaterial(texName);
}

void MeshRenderer::SetColor(const Vec4& color) {
	material_->SetColor(color);
}
