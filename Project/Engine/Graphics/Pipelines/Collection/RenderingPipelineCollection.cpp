#include "RenderingPipelineCollection.h"

/// engine
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/RendererComponents/Sprite/SpriteRenderer.h"

/// pipelines
#include "../Render/Mesh/MeshRenderingPipeline.h"
#include "../Render/Mesh/SkinMeshRenderingPipeline.h"
#include "../Render/Effect/EffectRenderingPipeline.h"
#include "../Render/Primitive/Line2DRenderingPipeline.h"
#include "../Render/Primitive/Line3DRenderingPipeline.h"
#include "../Render/Sprite/SpriteRenderingPipeline.h"
#include "../Render/Gizmo/GizmoRenderingPipeline.h"
#include "../Render/MeshShaderTest/MeshShaderTest.h"
#include "../Render/Skybox/SkyboxRenderingPipeline.h"
#include "../Render/Terrain/TerrainRenderingPipeline.h"

/// post process
#include "../PostProcess/PerObject/Light/PostProcessLighting.h"
#include "../PostProcess/Screen/Grayscale/PostProcessGrayscale.h"

RenderingPipelineCollection::RenderingPipelineCollection(ShaderCompiler* _shaderCompiler, DxManager* _dxManager, EntityComponentSystem* _pEntityComponentSystem, GraphicsResourceCollection* _graphicsResourceCollection)
	: shaderCompiler_(_shaderCompiler), dxManager_(_dxManager), pEntityComponentSystem_(_pEntityComponentSystem), graphicsResourceCollection_(_graphicsResourceCollection) {}

RenderingPipelineCollection::~RenderingPipelineCollection() {}

void RenderingPipelineCollection::Initialize() {

	/// generate rendering pipeline
	Generate2DRenderingPipeline<Line2DRenderingPipeline>();
	Generate2DRenderingPipeline<SpriteRenderingPipeline>(graphicsResourceCollection_);

	Generate3DRenderingPipeline<Line3DRenderingPipeline>();
	Generate3DRenderingPipeline<SkyboxRenderingPipeline>(graphicsResourceCollection_);
	Generate3DRenderingPipeline<TerrainRenderingPipeline>(graphicsResourceCollection_);
	//Generate3DRenderingPipeline<MeshShaderTest>();
	Generate3DRenderingPipeline<MeshRenderingPipeline>(graphicsResourceCollection_);
	Generate3DRenderingPipeline<SkinMeshRenderingPipeline>(graphicsResourceCollection_);
	Generate3DRenderingPipeline<EffectRenderingPipeline>(graphicsResourceCollection_);
	Generate3DRenderingPipeline<GizmoRenderingPipeline>();

	/// post process
	GeneratePostProcessPipeline<PostProcessLighting>();
	//GeneratePostProcessPipeline<PostProcessGrayscale>();
}

void RenderingPipelineCollection::DrawEntities(Camera* _3dCamera, Camera* _2dCamera) {
	if (_3dCamera) {
		for (auto& renderer : renderer3ds_) {
			renderer->Draw(dxManager_->GetDxCommand(), pEntityComponentSystem_, _3dCamera);
		}
	} else {
		Console::Log("RenderingPipelineCollection::DrawEntities: 3D Camera is null");
	}

	if (_2dCamera) {
		for (auto& renderer : renderer2ds_) {
			renderer->Draw(dxManager_->GetDxCommand(), pEntityComponentSystem_, _2dCamera);
		}
	} else {
		Console::Log("RenderingPipelineCollection::DrawEntities: 2D Camera is null");
	}
}


void RenderingPipelineCollection::ExecutePostProcess(const std::string& _sceneTextureName) {
	for (auto& postProcess : postProcesses_) {
		postProcess->Execute(_sceneTextureName, dxManager_->GetDxCommand(), graphicsResourceCollection_, pEntityComponentSystem_);
	}
}
