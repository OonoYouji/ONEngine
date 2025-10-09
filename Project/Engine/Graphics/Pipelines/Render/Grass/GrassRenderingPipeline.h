#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Grass/Grass.h"

/// /////////////////////////////////////////////////
/// 草を描画するパイプライン
/// /////////////////////////////////////////////////
class GrassRenderingPipeline : public IRenderingPipeline {

	enum ROOT_PARAM {
		ROOT_PARAM_VIEW_PROJECTION,
		ROOT_PARAM_BLADES,
		ROOT_PARAM_TIME,
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GrassRenderingPipeline();
	~GrassRenderingPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(class ECSGroup* _ecs, const std::vector<class GameEntity*>& _entities, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	StructuredBuffer<BladeInstance> bladesBuffer_;
	StructuredBuffer<float> timeBuffer_;

};

