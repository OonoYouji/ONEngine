﻿#pragma once

/// engine
#include "../Interface/IEditorCompute.h"

/// /////////////////////////////////////////////////
/// ボクセル地形を編集するためのパイプラインを起動するクラス
/// /////////////////////////////////////////////////
namespace ONEngine {

class VoxelTerrainEditorComputePipeline : public IEditorCompute {

	enum ROOT_PARAM {
		CBV_TERRAIN_INFO,
		CBV_VIEW_PROJECTION,
		CBV_CAMERA,
		CBV_INPUT_INFO,
		CBV_EDITOR_INFO,
		SRV_CHUNKS,
		SRV_WORLD_TEXTURE,
		UAV_VOXEL_TEXTURES
	};

public:
	/// =========================================
	/// public : methods
	/// =========================================

	VoxelTerrainEditorComputePipeline();
	~VoxelTerrainEditorComputePipeline() override;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Execute(class EntityComponentSystem* _ecs, class DxCommand* _dxCommand, class AssetCollection* _assetCollection) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

	class DxManager* pDxManager_ = nullptr;

};


} /// ONEngine
