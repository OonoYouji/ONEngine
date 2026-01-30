#pragma once


/// engine
#include "../../Interface/IRenderingPipeline.h"



namespace ONEngine {

class AssetCollection;


/// ///////////////////////////////////////////////////
/// ボクセル地形の編集用ブラシのプレビューを行う描画パイプライン
/// ///////////////////////////////////////////////////
class VoxelTerrainBrushPreviewRenderingPipeline : public IRenderingPipeline {

	enum ROOT_PARAM {
		CBV_VOXEL_TERRAIN_INFO,
		CBV_VIEW_PROJECTION,
		CBV_CAMERA_POSITION,
		CBV_LOD_INFO,
		CBV_MATERIAL,
	};


public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	VoxelTerrainBrushPreviewRenderingPipeline(AssetCollection* assetCollection);
	~VoxelTerrainBrushPreviewRenderingPipeline() override;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	AssetCollection* pAssetCollection_ = nullptr;

};

} /// namespace ONEngine