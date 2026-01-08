#include "RenderingPipelineCollection.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/RendererComponents/Sprite/SpriteRenderer.h"

/// pipelines
#include "../Render/Mesh/MeshRenderingPipeline.h"
#include "../Render/Mesh/DissolveMeshRenderingPipeline.h"
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
#include "../Render/VoxelTerrain/VoxelTerrainVertexCreatePipeline.h"
#include "../Render/VoxelTerrain/VoxelTerrainVertexShaderRenderingPipeline.h"
#include "../Render/VoxelTerrain/VoxelTerrainRenderingPipeline.h"

/// post process
#include "../PostProcess/PerObject/Light/PostProcessLighting.h"
#include "../PostProcess/PerObject/Grayscale/PostProcessGrayscalePerObject.h"
#include "../PostProcess/PerObject/Blur/PostProcessGaussianBlurPerObject.h"
#include "../PostProcess/Screen/Grayscale/PostProcessGrayscale.h"
#include "../PostProcess/Screen/RadialBlur/PostProcessRadialBlur.h"
#include "../PostProcess/Screen/Shadow/PostProcessShadowApply.h"
#include "../PostProcess/PerObject/TerrainBrush/PostProcessTerrainBrush.h"

RenderingPipelineCollection::RenderingPipelineCollection(ShaderCompiler* _shaderCompiler, DxManager* _dxm, EntityComponentSystem* _pEntityComponentSystem, AssetCollection* _assetCollection)
	: pShaderCompiler_(_shaderCompiler), pDxManager_(_dxm), pEntityComponentSystem_(_pEntityComponentSystem), pAssetCollection_(_assetCollection) {
}

RenderingPipelineCollection::~RenderingPipelineCollection() {}

void RenderingPipelineCollection::Initialize() {

	/// ----- 2D用のパイプラインを生成 ----- ///
	Generate2DRenderingPipeline<Line2DRenderingPipeline>();
	Generate2DRenderingPipeline<SpriteRenderingPipeline>(pAssetCollection_);

	/// ----- 3D用のパイプラインを生成 ----- ///
	Generate3DRenderingPipeline<Line3DRenderingPipeline>();
	Generate3DRenderingPipeline<SkyboxRenderingPipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<TerrainRenderingPipeline>(pAssetCollection_);
	//Generate3DRenderingPipeline<VoxelTerrainVertexCreatePipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<VoxelTerrainRenderingPipeline>(pAssetCollection_);
	//Generate3DRenderingPipeline<VoxelTerrainVertexShaderRenderingPipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<TerrainProceduralRenderingPipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<RiverRenderingPipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<MeshRenderingPipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<DissolveMeshRenderingPipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<SkinMeshRenderingPipeline>(pAssetCollection_);
#ifdef DEBUG_MODE
	/// Debug用にスケルトンを描画するパイプラインを追加
	Generate3DRenderingPipeline<SkinMeshSkeletonRenderingPipeline>();
#endif // DEBUG_MODE
	Generate3DRenderingPipeline<EffectRenderingPipeline>(pAssetCollection_);
	Generate3DRenderingPipeline<GrassRenderingPipeline>(pAssetCollection_);

	/// Gizmoは最後に描画する
	Generate3DRenderingPipeline<GizmoRenderingPipeline>();



	/// ----- オブジェクトごとのポストエフェクトのパイプラインを生成 ----- ///
	GeneratePostProcessPipeline<PostProcessLighting>();
	GeneratePostProcessPipeline<PostProcessGrayscalePerObject>();
	GeneratePostProcessPipeline<PostProcessTerrainBrush>();
	GeneratePostProcessPipeline<PostProcessGaussianBlurPerObject>();

	/// ----- スクリーンにかける用のポストエフェクトのパイプラインを生成 ----- ///
	GeneratePostProcessPipeline<PostProcessGrayscale>();
	GeneratePostProcessPipeline<PostProcessRadialBlur>();
	GeneratePostProcessPipeline<PostProcessShadowApply>();
}

void RenderingPipelineCollection::PreDrawEntities(CameraComponent* _3dCamera, CameraComponent* _2dCamera) {

	/// ----- すべてのPipelineのPreDrawを実行する ----- ///
	ECSGroup* ecsGroup = pEntityComponentSystem_->GetCurrentGroup();

	/// 2d,3d 両方ともカメラが有効かチェックしてから描画する
	if (IsEnableCamera(_3dCamera)) {
		for (auto& renderer : renderer3ds_) {
			renderer->PreDraw(ecsGroup, _3dCamera, pDxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 3D Camera is null");
	}

	if (IsEnableCamera(_2dCamera)) {
		for (auto& renderer : renderer2ds_) {
			renderer->PreDraw(ecsGroup, _2dCamera, pDxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 2D Camera is null");
	}
}

void RenderingPipelineCollection::DrawEntities(CameraComponent* _3dCamera, CameraComponent* _2dCamera) {

	/// シーンを描画するので現在のGroupを使用する
	ECSGroup* ecsGroup = pEntityComponentSystem_->GetCurrentGroup();

	/// 2d,3d 両方ともカメラが有効かチェックしてから描画する
	if (IsEnableCamera(_3dCamera)) {
		for (auto& renderer : renderer3ds_) {
			renderer->Draw(ecsGroup, _3dCamera, pDxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 3D Camera is null");
	}

	if (IsEnableCamera(_2dCamera)) {
		for (auto& renderer : renderer2ds_) {
			renderer->Draw(ecsGroup, _2dCamera, pDxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 2D Camera is null");
	}
}

void RenderingPipelineCollection::DrawSelectedPrefab(CameraComponent* _3dCamera, CameraComponent* _2dCamera) {
	/// ----- 選択されているPrefabの描画 ----- ///

	/// デバッグ用のGroupを使用する
	ECSGroup* ecsGroup = pEntityComponentSystem_->GetECSGroup("Debug");

	/// 2d,3d 両方ともカメラが有効かチェックしてから描画する
	if (IsEnableCamera(_3dCamera)) {
		for (auto& renderer : renderer3ds_) {
			renderer->Draw(ecsGroup, _3dCamera, pDxManager_->GetDxCommand());
		}
	} else {
		Console::LogError("RenderingPipelineCollection::DrawEntities: 3D Camera is null");
	}

	if (IsEnableCamera(_2dCamera)) {
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

bool RenderingPipelineCollection::IsEnableCamera(const CameraComponent* _camera) const {
	/*
	* チェック項目
	* 1, カメラのポインタが有効
	* 2, Componentの有効フラグがtrue
	* 3, Bufferとして利用できるViewProjectionがあるか
	*/

	return _camera && _camera->enable && _camera->IsMakeViewProjection();
}
