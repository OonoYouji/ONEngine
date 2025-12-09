#pragma once

#include "../Interface/IEditorCompute.h"

/// /////////////////////////////////////////////////
/// 川に沿って地形を変形させるCSPipelineを起動させるクラス
/// /////////////////////////////////////////////////
class RiverTerrainAbjustPipeline : public IEditorCompute {

	enum ROOT_PARAM {
		CBV_PARAMS,
		UAV_TERRAIN_VERTICES,
		SRV_RIVER_VERTICES,
		SRV_RIVER_INDICES,
	};

public:
	/// =========================================
	/// public : methods
	/// =========================================

	RiverTerrainAbjustPipeline();
	~RiverTerrainAbjustPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Execute(class EntityComponentSystem* _ecs, class DxCommand* _dxCommand, class AssetCollection* _assetCollection) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================
};

