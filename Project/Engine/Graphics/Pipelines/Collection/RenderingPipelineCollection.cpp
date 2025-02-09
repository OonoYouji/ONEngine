#include "RenderingPipelineCollection.h"

/// engine
#include "Engine/Entity/Collection/EntityCollection.h"

/// pipelines
#include "../Mesh/MeshRenderingPipeline.h"
#include "../Primitive/Line2DRenderingPipeline.h"

/// renderers
#include "Engine/Component/RendererComponents/Primitive/Line2DRenderer.h"


RenderingPipelineCollection::RenderingPipelineCollection(ShaderCompiler* _shaderCompiler, DxManager* _dxManager, EntityCollection* _entityCollection)
	: shaderCompiler_(_shaderCompiler), dxManager_(_dxManager), entityCollection_(_entityCollection) {}

RenderingPipelineCollection::~RenderingPipelineCollection() {}

void RenderingPipelineCollection::Initialize() {

	/// generate rendering pipeline
	GenerateRenderingPipeline<Line2DRenderingPipeline>();
	GenerateRenderingPipeline<MeshRenderingPipeline>();

}

void RenderingPipelineCollection::DrawEntities() {
	for (auto& renderer : renderers_) {
		renderer.second->PreDraw(dxManager_->GetDxCommand());
	}

	/// すべての entity の描画処理
	const std::vector<std::unique_ptr<IEntity>>& entities = entityCollection_->GetEntities();
	for (auto& entity : entities) {

		Line2DRenderer* lineRenderer = entity->GetComponent<Line2DRenderer>();
		if (lineRenderer) {
			lineRenderer->PushBackRenderingData(this);
		}
	}


	for (auto& renderer : renderers_) {
		renderer.second->PostDraw(dxManager_->GetDxCommand());
	}
}
