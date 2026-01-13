#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"


namespace ONEngine {

class AssetCollection;

/// ///////////////////////////////////////////////////
/// 地形のチャンク間を滑らかに接続するためのレンダリングパイプライン
/// ///////////////////////////////////////////////////
class VoxelTerrainTransvoxelRenderingPipeline : public IRenderingPipeline {
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

};


} /// namespace ONEngine