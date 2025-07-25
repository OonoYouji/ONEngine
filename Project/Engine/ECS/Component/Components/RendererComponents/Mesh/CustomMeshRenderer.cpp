#include "CustomMeshRenderer.h"

/// engine
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

CustomMeshRenderer::CustomMeshRenderer() {
	material_.color = Vector4::kWhite;
	material_.postEffectFlags = PostEffectFlags_Lighting;
}

CustomMeshRenderer::~CustomMeshRenderer() {}

void CustomMeshRenderer::MeshRecreate(DxDevice* _pDxDevice) {
	mesh_.CreateBuffer(_pDxDevice);
	mesh_.VertexBufferMapping();
	mesh_.IndexBufferMapping();
}

void CustomMeshRenderer::VertexMemcpy() {
	mesh_.MemcpyVertexData();
}

void CustomMeshRenderer::SetVertices(const std::vector<Mesh::VertexData>& _vertices) {
	mesh_.SetVertices(_vertices);
}

void CustomMeshRenderer::SetIndices(const std::vector<uint32_t>& _indices) {
	mesh_.SetIndices(_indices);
}

void CustomMeshRenderer::SetTexturePath(const std::string& _path) {
	texturePath_ = _path;
}

void CustomMeshRenderer::SetColor(const Vector4& _color) {
	material_.color = _color;
}

void CustomMeshRenderer::SetIsVisible(bool _isVisible) {
	isVisible_ = _isVisible;
}

void CustomMeshRenderer::SetIsBufferRecreate(bool _isBufferRecreate) {
	isBufferRecreate_ = _isBufferRecreate;
}

const std::string& CustomMeshRenderer::GetTexturePath() const {
	return texturePath_;
}

const Vector4& CustomMeshRenderer::GetColor() const {
	return material_.color;
}

const Mesh* CustomMeshRenderer::GetMesh() const {
	return &mesh_;
}

bool CustomMeshRenderer::GetIsVisible() const {
	return isVisible_;
}

bool CustomMeshRenderer::GetIsBufferRecreate() const {
	return isBufferRecreate_;
}

const Material& CustomMeshRenderer::GetMaterial() {
	return material_;
}
