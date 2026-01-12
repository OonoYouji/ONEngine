#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"


namespace ONEngine {

class AssetCollection;


/// ///////////////////////////////////////////////////
/// VertexShaderによるVoxelTerrain描画パイプライン
/// ///////////////////////////////////////////////////
class VoxelTerrainVertexShaderRenderingPipeline : public IRenderingPipeline {

	enum ROOT_PARAM {
		CBV_VIEW_PROJECTION,
		CBV_MATERIAL
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	VoxelTerrainVertexShaderRenderingPipeline(AssetCollection* _ac);
	~VoxelTerrainVertexShaderRenderingPipeline();

	/// --------------- override methods --------------- ///
	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// --------------- other class pointers --------------- ///
	class AssetCollection* pAssetCollection_;
	class DxManager* pDxManager_;

};

} /// namespace ONEngine