#include "MeshRenderer.h"

/// engine
#include "Engine/Graphics/Pipelines/Collection/RenderingPipelineCollection.h"
#include "Engine/Entity/Interface/IEntity.h"

MeshRenderer::MeshRenderer() {
	renderingData_.renderMeshId = 0;
	renderingData_.transformPtr = owner_->GetTransform();
}

MeshRenderer::~MeshRenderer() {}

void MeshRenderer::PushBackRenderingData(RenderingPipelineCollection* _collection) {
	MeshRenderingPipeline* renderer = _collection->GetRenderingPipeline<MeshRenderingPipeline>();
	renderer->PushBackRenderingData(&renderingData_);
}
