#include "RenderingPipelineCollection.h"

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Asset/Collection/AssetCollection.h"
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
#include "../Render/River/RiverRenderingPipeline.h"
#include "../Render/Grass/GrassRenderingPipeline.h"

/// post process
#include "../PostProcess/PerObject/Light/PostProcessLighting.h"
#include "../PostProcess/PerObject/Grayscale/PostProcessGrayscalePerObject.h"
#include "../PostProcess/PerObject/Blur/PostProcessGaussianBlurPerObject.h"
#include "../PostProcess/Screen/Grayscale/PostProcessGrayscale.h"
#include "../PostProcess/Screen/RadialBlur/PostProcessRadialBlur.h"
#include "../PostProcess/PerObject/TerrainBrush/PostProcessTerrainBrush.h"

RenderingPipelineCollection::RenderingPipelineCollection(ShaderCompiler* _shaderCompiler, DxManager* _dxm, EntityComponentSystem* _pEntityComponentSystem, AssetCollection* _assetCollection)
	: pShaderCompiler_(_shaderCompiler), pDxManager_(_dxm), pEntityComponentSystem_(_pEntityComponentSystem), pAssetCollection_(_assetCollection) {
}

RenderingPipelineCollection::~RenderingPipelineCollection() {}

void RenderingPipelineCollection::Initialize() {

	/// 2D
	Generate2DRenderingPipeline<Line2DRenderingPipeline>();
	Generate2DRenderingPipeline<SpriteRenderingPipeline>(pAssetCollection_);

	/// 3D 
	Generate3DRenderingPipeline<Line3DRenderingPipeline>();
	Generate3DRenderingPipeline<SkyboxRenderingPipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<TerrainRenderingPipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<TerrainProceduralRenderingPipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<RiverRenderingPipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<MeshRenderingPipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<SkinMeshRenderingPipeline>(pAssetCollection_);
#ifdef DEBUG_MODE
	Generate3DRenderingPipeline<SkinMeshSkeletonRenderingPipeline>();
#endif // DEBUG_MODE
	Generate3DRenderingPipeline<EffectRenderingPipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<GizmoRenderingPipeline>();
	Generate3DRenderingPipeline<GrassRenderingPipeline>(pAssetCollection_);

	/// post process - per object
	GeneratePostProcessPipeline<PostProcessLighting>();
	GeneratePostProcessPipeline<PostProcessGrayscalePerObject>();
	GeneratePostProcessPipeline<PostProcessTerrainBrush>();
	GeneratePostProcessPipeline<PostProcessGaussianBlurPerObject>();

	/// post process - screen
	GeneratePostProcessPipeline<PostProcessGrayscale>();
	GeneratePostProcessPipeline<PostProcessRadialBlur>();
}

void RenderingPipelineCollection::PreDrawEntities(CameraComponent* _3dCamera, CameraComponent* _2dCamera) {
	ECSGroup* ecsGroup = pEntityComponentSystem_->GetCurrentGroup();

	if (_3dCamera && _3dCamera->IsMakeViewProjection()) {
		for (auto& renderer : renderer3ds_) {
			renderer->PreDraw(ecsGroup, _3dCamera, pDxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 3D Camera is null");
	}

	if (_2dCamera && _2dCamera->IsMakeViewProjection()) {
		for (auto& renderer : renderer2ds_) {
			renderer->PreDraw(ecsGroup, _2dCamera, pDxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 2D Camera is null");
	}
}

void RenderingPipelineCollection::DrawEntities(CameraComponent* _3dCamera, CameraComponent* _2dCamera) {
	ECSGroup* ecsGroup = pEntityComponentSystem_->GetCurrentGroup();

	if (_3dCamera && _3dCamera->IsMakeViewProjection()) {
		for (auto& renderer : renderer3ds_) {
			renderer->Draw(ecsGroup, _3dCamera, pDxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 3D Camera is null");
	}

	if (_2dCamera && _2dCamera->IsMakeViewProjection()) {
		for (auto& renderer : renderer2ds_) {
			renderer->Draw(ecsGroup, _2dCamera, pDxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 2D Camera is null");
	}
}

void RenderingPipelineCollection::DrawSelectedPrefab(CameraComponent* _3dCamera, CameraComponent* _2dCamera) {
	ECSGroup* ecsGroup = pEntityComponentSystem_->GetECSGroup("Debug");

	if (_3dCamera && _3dCamera->IsMakeViewProjection()) {
		for (auto& renderer : renderer3ds_) {
			renderer->Draw(ecsGroup, _3dCamera, pDxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 3D Camera is null");
	}

	if (_2dCamera && _2dCamera->IsMakeViewProjection()) {
		for (auto& renderer : renderer2ds_) {
			renderer->Draw(ecsGroup, _2dCamera, pDxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 2D Camera is null");
	}
}


void RenderingPipelineCollection::ExecutePostProcess(const std::string& _sceneTextureName) {
	for (auto& postProcess : postProcesses_) {
		postProcess->Execute(_sceneTextureName, pDxManager_->GetDxCommand(), pAssetCollection_, pEntityComponentSystem_);
	}
}
