#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

/// ///////////////////////////////////////////////////
/// gizmoの表示pipeline
/// ///////////////////////////////////////////////////
class GizmoRenderingPipeline : public IRenderingPipeline {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GizmoRenderingPipeline();
	~GizmoRenderingPipeline() = default;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(DxCommand* _dxCommand, class EntityComponentSystem* _entityComponentSystem, class Camera* _camera) override;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

};

