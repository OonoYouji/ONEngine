#include "MeshRenderer.h"

/// engine
#include "Engine/Graphics/Pipelines/Collection/RenderingPipelineCollection.h"

MeshRenderer::MeshRenderer() {
	renderingData_.renderMeshId = 0;

}

MeshRenderer::~MeshRenderer() {}

void MeshRenderer::PushBackRenderingData(RenderingPipelineCollection* _collection) {
	MeshRenderingPipeline* renderer = _collection->GetRenderingPipeline<MeshRenderingPipeline>();
	renderer->PushBackRenderingData(&renderingData_);
}
