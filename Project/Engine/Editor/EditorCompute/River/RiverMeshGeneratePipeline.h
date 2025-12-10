#pragma once

#include "../Interface/IEditorCompute.h"

/// //////////////////////////////////////////
/// 川のメッシュを作成するpipeline
/// //////////////////////////////////////////
namespace Editor {

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

	void Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) override;
	void Execute(ONEngine::EntityComponentSystem* _ecs, ONEngine::DxCommand* _dxCommand, ONEngine::AssetCollection* _assetCollection) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

	ONEngine::DxManager* pDxManager_;

};

} /// Editor
