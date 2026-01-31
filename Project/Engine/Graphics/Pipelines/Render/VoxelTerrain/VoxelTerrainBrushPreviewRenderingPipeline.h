#pragma once


/// engine
#include "../../Interface/IRenderingPipeline.h"

#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"


namespace ONEngine {

class AssetCollection;
class DxManager;


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
		CBV_BRUSH_INFO,
		SRV_CHUNKS,
		SRV_WORLD_POSITION_TEXTURE,
		SRV_VOXEL_TERRAIN_TEXTURE3D,
	};

	struct BrushInfo {
		Vector2 mouseScreenPos;
		uint32_t brushRadius;
		float brushStrength;
	};


public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	VoxelTerrainBrushPreviewRenderingPipeline(AssetCollection* assetCollection);
	~VoxelTerrainBrushPreviewRenderingPipeline() override;

	void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) override;
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;

	bool CheckVoxelTerrainBufferEnabled(class VoxelTerrain* vt);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	AssetCollection* pAssetCollection_ = nullptr;
	DxManager* pDxm_ = nullptr;

	ConstantBuffer<BrushInfo> cBufferBrushInfo_;
};

} /// namespace ONEngine