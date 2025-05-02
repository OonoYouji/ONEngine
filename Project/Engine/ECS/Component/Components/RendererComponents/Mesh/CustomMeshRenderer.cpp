#include "CustomMeshRenderer.h"

/// engine
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

CustomMeshRenderer::CustomMeshRenderer() {}
CustomMeshRenderer::~CustomMeshRenderer() {}

void CustomMeshRenderer::MeshRecreate(DxDevice* _pDxDevice) {
	mesh_.CreateBuffer(_pDxDevice);
	mesh_.VertexBufferMapping();
	mesh_.IndexBufferMapping();
}

void CustomMeshRenderer::SetVertices(const std::vector<Mesh::VertexData>& _vertices) {
	mesh_.SetVertices(_vertices);
	//mesh_.VertexBufferMapping();
}

void CustomMeshRenderer::SetIndices(const std::vector<uint32_t>& _indices) {
	mesh_.SetIndices(_indices);
	//mesh_.IndexBufferMapping();
}
