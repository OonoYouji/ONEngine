#include "MeshRenderer.h"

/// engine
#include "Engine/Graphics/Pipelines/Collection/RenderingPipelineCollection.h"

MeshRenderer::MeshRenderer() {
	SetMeshPath("./Assets/Models/primitive/cube.obj");
	SetTexturePath("./Packages/Textures/white.png");
}

MeshRenderer::~MeshRenderer() {}
