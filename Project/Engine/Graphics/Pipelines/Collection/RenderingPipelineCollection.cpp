#include "RenderingPipelineCollection.h"

/// engine
#include "Engine/Entity/Collection/EntityCollection.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

/// pipelines
#include "../Mesh/MeshRenderingPipeline.h"
#include "../Primitive/Line2DRenderingPipeline.h"

/// renderers
#include "Engine/Component/RendererComponents/Primitive/Line2DRenderer.h"


RenderingPipelineCollection::RenderingPipelineCollection(ShaderCompiler* _shaderCompiler, DxManager* _dxManager, EntityCollection* _entityCollection, GraphicsResourceCollection* _graphicsResourceCollection)
	: shaderCompiler_(_shaderCompiler), dxManager_(_dxManager), entityCollection_(_entityCollection), graphicsResourceCollection_(_graphicsResourceCollection) {}

RenderingPipelineCollection::~RenderingPipelineCollection() {}

void RenderingPipelineCollection::Initialize() {

	/// generate rendering pipeline
	GenerateRenderingPipeline<Line2DRenderingPipeline>();
	GenerateRenderingPipeline<MeshRenderingPipeline>(graphicsResourceCollection_, entityCollection_);

}

void RenderingPipelineCollection::DrawEntities() {
	for (auto& renderer : renderers_) {
		renderer.second->Draw(dxManager_->GetDxCommand(), entityCollection_);
	}
}
