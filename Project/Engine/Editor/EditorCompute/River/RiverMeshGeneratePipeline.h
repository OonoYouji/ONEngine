#pragma once

#include "../Interface/IEditorCompute.h"

/// //////////////////////////////////////////
/// 川のメッシュを作成するpipeline
/// //////////////////////////////////////////
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

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Execute(class EntityComponentSystem* _ecs, class DxCommand* _dxCommand, class GraphicsResourceCollection* _resourceCollection) override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

	class DxManager* pDxManager_;

};