#include "MeshRenderer.h"

/// engine
#include "Engine/Graphics/Pipelines/Collection/RenderingPipelineCollection.h"
#include "Engine/ECS/Entity/Interface/IEntity.h"

MeshRenderer::MeshRenderer() {
	SetMeshPath("Assets/Models/primitive/cube.obj");
}

MeshRenderer::~MeshRenderer() {}
