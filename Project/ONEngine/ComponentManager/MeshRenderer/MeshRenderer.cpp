#include "MeshRenderer.h"

#include "GraphicManager/ModelManager/ModelManager.h"
#include "GameObjectManager/BaseGameObject.h"


void MeshRenderer::Initialize() {
	pTransform_ = GetOwner()->GetComponent<Transform>();
}

void MeshRenderer::Draw() {
	model_->Draw(pTransform_, nullptr, material_.get(), fillMode_);
}



void MeshRenderer::SetModel(const std::string& _filePath) {
	model_ = ModelManager::Load(_filePath);
}

void MeshRenderer::SetModel(Model* _model) {
	model_ = _model;
}

void MeshRenderer::SetMaterial(const std::string& _texName) {
	if(!material_) {
		material_.reset(new Material);
	}

	material_->CreateMaterial(_texName);
}

void MeshRenderer::SetIsLighting(bool _isLighting) {
	material_->SetIsLighting(_isLighting);
}

void MeshRenderer::SetUVScale(const Vec2& _scale) {
	material_->SetScale(_scale);
}

void MeshRenderer::SetUVRotate(float _rotate) {
	material_->SetRotate(_rotate);
}

void MeshRenderer::SetUVPosition(const Vec2& _position) {
	material_->SetPosition(_position);
}

void MeshRenderer::SetColor(const Vec4& _color) {
	material_->SetColor(_color);
}
