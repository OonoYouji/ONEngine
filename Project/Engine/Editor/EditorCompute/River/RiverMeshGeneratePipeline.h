﻿#pragma once

#include "../Interface/IEditorCompute.h"

/// //////////////////////////////////////////
/// 川のメッシュを作成するpipeline
/// //////////////////////////////////////////
namespace ONEngine {

class RiverMeshGeneratePipeline : public IEditorCompute {

	enum ROOT_PARAM {
		CBV_PARAMS,
		SRV_CONTROL_POINTS,
		UAV_VERTICES,
		UAV_INDICES,
	};

public:
	/// =========================================
	/// public : methods
	/// =========================================

	RiverMeshGeneratePipeline();
	~RiverMeshGeneratePipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Execute(class EntityComponentSystem* _ecs, class DxCommand* _dxCommand, class AssetCollection* _assetCollection) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

	class DxManager* pDxManager_;

};

} /// ONEngine
