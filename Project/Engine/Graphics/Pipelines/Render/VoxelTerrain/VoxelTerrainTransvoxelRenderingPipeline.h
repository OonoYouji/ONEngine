#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"


namespace ONEngine {

class AssetCollection;
class DxManager;

/// ///////////////////////////////////////////////////
/// 地形のチャンク間を滑らかに接続するためのレンダリングパイプライン
/// ///////////////////////////////////////////////////
class VoxelTerrainTransvoxelRenderingPipeline : public IRenderingPipeline {

	enum ROOT_PARAM {
		CBV_VOXEL_TERRAIN_INFO = 0,
		CBV_VIEW_PROJECTION,
		CBV_CAMERA_POSITION,
		CBV_LOD_INFO,
		CBV_MATERIAL,
		SRV_CHUNK_ARRAY,
		SRV_VOXEL_TERRAIN_TEXTURE3D,
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	VoxelTerrainTransvoxelRenderingPipeline(AssetCollection* _ac);
	~VoxelTerrainTransvoxelRenderingPipeline() override;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	AssetCollection* pAssetCollection_;
	DxManager* pDxManager_;

};


} /// namespace ONEngine