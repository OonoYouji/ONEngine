#include "MeshRenderer.h"

/// engine
#include "Engine/Graphics/Pipelines/Collection/RenderingPipelineCollection.h"
#include "Engine/Entity/Interface/IEntity.h"

MeshRenderer::MeshRenderer() {
	renderingData_.renderMeshId = 0;
	renderingData_.meshRenderer = this;
}

MeshRenderer::~MeshRenderer() {}
