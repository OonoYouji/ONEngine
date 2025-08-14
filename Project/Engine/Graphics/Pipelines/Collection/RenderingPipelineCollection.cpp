#include "RenderingPipelineCollection.h"

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/RendererComponents/Sprite/SpriteRenderer.h"

/// pipelines
#include "../Render/Mesh/MeshRenderingPipeline.h"
#include "../Render/Mesh/SkinMeshRenderingPipeline.h"
#include "../Render/Mesh/SkinMeshSkeletonRenderingPipeline.h"
#include "../Render/Effect/EffectRenderingPipeline.h"
#include "../Render/Primitive/Line2DRenderingPipeline.h"
#include "../Render/Primitive/Line3DRenderingPipeline.h"
#include "../Render/Sprite/SpriteRenderingPipeline.h"
#include "../Render/Gizmo/GizmoRenderingPipeline.h"
#include "../Render/Skybox/SkyboxRenderingPipeline.h"
#include "../Render/Terrain/TerrainRenderingPipeline.h"
#include "../Render/Terrain/TerrainProceduralRenderingPipeline.h"

/// post process
#include "../PostProcess/PerObject/Light/PostProcessLighting.h"
#include "../PostProcess/PerObject/Grayscale/PostProcessGrayscalePerObject.h"
#include "../PostProcess/PerObject/Blur/PostProcessGaussianBlurPerObject.h"
#include "../PostProcess/Screen/Grayscale/PostProcessGrayscale.h"
#include "../PostProcess/Screen/RadialBlur/PostProcessRadialBlur.h"

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
	Generate3DRenderingPipeline<TerrainProceduralRenderingPipeline>(graphicsResourceCollection_);
	Generate3DRenderingPipeline<MeshRenderingPipeline>(graphicsResourceCollection_);
	Generate3DRenderingPipeline<SkinMeshRenderingPipeline>(graphicsResourceCollection_);
#ifdef DEBUG_MODE
	Generate3DRenderingPipeline<SkinMeshSkeletonRenderingPipeline>();
#endif // DEBUG_MODE
	Generate3DRenderingPipeline<EffectRenderingPipeline>(graphicsResourceCollection_);
	Generate3DRenderingPipeline<GizmoRenderingPipeline>();

	/// post process - per object
	GeneratePostProcessPipeline<PostProcessLighting>();
	GeneratePostProcessPipeline<PostProcessGrayscalePerObject>();
	//GeneratePostProcessPipeline<PostProcessGaussianBlurPerObject>();

	/// post process - screen
	GeneratePostProcessPipeline<PostProcessGrayscale>();
	GeneratePostProcessPipeline<PostProcessRadialBlur>();
}

void RenderingPipelineCollection::PreDrawEntities(CameraComponent* _3dCamera, CameraComponent* _2dCamera) {
	ECSGroup* ecsGroup = pEntityComponentSystem_->GetCurrentGroup();

	if (_3dCamera && _3dCamera->IsMakeViewProjection()) {
		for (auto& renderer : renderer3ds_) {
			renderer->PreDraw(ecsGroup, _3dCamera, dxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 3D Camera is null");
	}

	if (_2dCamera && _2dCamera->IsMakeViewProjection()) {
		for (auto& renderer : renderer2ds_) {
			renderer->PreDraw(ecsGroup, _2dCamera, dxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 2D Camera is null");
	}
}

void RenderingPipelineCollection::DrawEntities(CameraComponent* _3dCamera, CameraComponent* _2dCamera) {

	ECSGroup* ecsGroup = pEntityComponentSystem_->GetCurrentGroup();

	std::vector<GameEntity*> entities;
	entities.reserve(ecsGroup->GetEntities().size());
	for (auto& entity : ecsGroup->GetEntities()) {
		if (entity.get() && entity->GetActive()) {
			entities.push_back(entity.get());
		}
	}


	if (_3dCamera && _3dCamera->IsMakeViewProjection()) {
		for (auto& renderer : renderer3ds_) {
			renderer->Draw(ecsGroup, entities, _3dCamera, dxManager_->GetDxCommand());
		}

	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 3D Camera is null");
	}

	if (_2dCamera && _2dCamera->IsMakeViewProjection()) {
		for (auto& renderer : renderer2ds_) {
			renderer->Draw(ecsGroup, entities, _2dCamera, dxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 2D Camera is null");
	}
}

void RenderingPipelineCollection::DrawSelectedPrefab(CameraComponent* _3dCamera, CameraComponent* _2dCamera) {

	std::vector<GameEntity*> entities;
	//GameEntity* prefabEntity = pEntityComponentSystem_->GetECSGroup()->GetPrefabEntity();
	//if (prefabEntity) {
	//	entities.push_back(prefabEntity);
	//}

	if (entities.empty()) {
		return;
	}

	ECSGroup* ecsGroup = pEntityComponentSystem_->GetCurrentGroup();


	if (_3dCamera) {
		for (auto& renderer : renderer3ds_) {
			renderer->Draw(ecsGroup, entities, _3dCamera, dxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 3D Camera is null");
	}

	if (_2dCamera) {
		for (auto& renderer : renderer2ds_) {
			renderer->Draw(ecsGroup, entities, _2dCamera, dxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 2D Camera is null");
	}
}


void RenderingPipelineCollection::ExecutePostProcess(const std::string& _sceneTextureName) {
	for (auto& postProcess : postProcesses_) {
		postProcess->Execute(_sceneTextureName, dxManager_->GetDxCommand(), graphicsResourceCollection_, pEntityComponentSystem_);
	}
}
