#pragma once

#include "../Interface/IEditorCompute.h"

namespace Editor {

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

	void Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) override;
	void Execute(ONEngine::EntityComponentSystem* _ecs, ONEngine::DxCommand* _dxCommand, ONEngine::AssetCollection* _assetCollection) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================
};

} /// Editor
