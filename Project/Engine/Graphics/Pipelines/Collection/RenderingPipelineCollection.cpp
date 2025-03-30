#include "RenderingPipelineCollection.h"

/// engine
#include "Engine/Entity/Collection/EntityCollection.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

/// pipelines
#include "../Render/Mesh/MeshRenderingPipeline.h"
#include "../Render/Primitive/Line2DRenderingPipeline.h"
#include "../Render/Primitive/Line3DRenderingPipeline.h"
#include "../Render/Sprite/SpriteRenderingPipeline.h"
#include "../PostProcess/Light/PostProcessLighting.h"


RenderingPipelineCollection::RenderingPipelineCollection(ShaderCompiler* _shaderCompiler, DxManager* _dxManager, EntityCollection* _entityCollection, GraphicsResourceCollection* _graphicsResourceCollection)
	: shaderCompiler_(_shaderCompiler), dxManager_(_dxManager), entityCollection_(_entityCollection), graphicsResourceCollection_(_graphicsResourceCollection) {}

RenderingPipelineCollection::~RenderingPipelineCollection() {}

void RenderingPipelineCollection::Initialize() {

	/// generate rendering pipeline
	GenerateRenderingPipeline<Line2DRenderingPipeline>();
	GenerateRenderingPipeline<Line3DRenderingPipeline>();
	GenerateRenderingPipeline<MeshRenderingPipeline>(graphicsResourceCollection_);
	GenerateRenderingPipeline<SpriteRenderingPipeline>(graphicsResourceCollection_);

	/// post process
	GeneratePostProcessPipeline<PostProcessLighting>();

}

void RenderingPipelineCollection::DrawEntities() {
	for (auto& renderer : renderers_) {
		renderer->Draw(dxManager_->GetDxCommand(), entityCollection_);
	}
}

void RenderingPipelineCollection::ExecutePostProcess() {
	for (auto& postProcess : postProcesses_) {
		postProcess->Execute(dxManager_->GetDxCommand(), graphicsResourceCollection_, entityCollection_);
	}
}
