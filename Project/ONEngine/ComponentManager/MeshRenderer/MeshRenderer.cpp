#include "MeshRenderer.h"

/// engine
#include "GraphicManager/TextureManager/TextureManager.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GameObjectManager/BaseGameObject.h"


void MeshRenderer::Initialize() {
	pTransform_ = GetOwner()->GetComponent<Transform>();
}

void MeshRenderer::Update() {
	if(material_) {
		material_->UpdateMatrix();
	}
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

void MeshRenderer::SetMaterial(const std::string& _filePath) {
	if(!material_) {
		material_.reset(new Material);
	}
	size_t dotPosition = _filePath.find('.');
	std::string name = _filePath;

	if(dotPosition != std::string::npos) {
		name.erase(dotPosition);
	}

	TextureManager::GetInstance()->Load(name, _filePath);
	material_->CreateMaterial(name);
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

void MeshRenderer::SetShiniess(float _shininess) {
	material_->SetSininess(_shininess);
}
