#include "CustomMeshRenderer.h"

/// engine
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

CustomMeshRenderer::CustomMeshRenderer() {
	frontMesh_ = model_.CreateMesh();
	model_.SetPath("Terrain");
}
CustomMeshRenderer::~CustomMeshRenderer() {}

void CustomMeshRenderer::SetVertices(const std::vector<Mesh::VertexData>& _vertices) {
	frontMesh_->SetVertices(_vertices);
}

void CustomMeshRenderer::SetIndices(const std::vector<uint32_t>& _indices) {
	frontMesh_->SetIndices(_indices);
}
