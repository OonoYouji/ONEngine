#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Shader/ComputePipeline.h"

/// ////////////////////////////////////
/// 地形に対してのプロシージャルレンダリングパイプライン
/// ////////////////////////////////////
class TerrainProceduralRenderingPipeline : public IRenderingPipeline {

	enum CP_ROOT_PARAM {
		CP_INSNTANCE_DATA,
		CP_SRV_VERTEX_TEXTURE
	};

	enum GP_ROOT_PARAM {
		GP_CBV_VIEW_PROJECTION,
		GP_SRV_INSNTANCE_DATA
	};

public:
	/// =====================================
	/// public : methods
	/// =====================================

	TerrainProceduralRenderingPipeline();
	~TerrainProceduralRenderingPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(class EntityComponentSystem* _ecs, const std::vector<IEntity*>& _entities, class CameraComponent* _camera, class DxCommand* _dxCommand) override;

private:
	/// =====================================
	/// private : objects
	/// =====================================

	std::unique_ptr<ComputePipeline> computePipeline_;

};

