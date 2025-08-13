#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Core/Utility/Math/Matrix4x4.h"
#include "Engine/Graphics/Shader/ComputePipeline.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

/// ////////////////////////////////////
/// 地形に対してのプロシージャルレンダリングパイプライン
/// ////////////////////////////////////
class TerrainProceduralRenderingPipeline : public IRenderingPipeline {

	/// @brief compute shader用root param index
	enum CP_ROOT_PARAM {
		CP_INSNTANCE_DATA,
		CP_SRV_VERTEX_TEXTURE,
		CP_SRV_SPLAT_BLEND_TEXTURE
	};

	/// @brief 
	enum GP_ROOT_PARAM {
		GP_CBV_VIEW_PROJECTION,
		GP_SRV_INSNTANCE_DATA,
		GP_SRV_TEXTURES
	};

	struct InstanceData {
		Matrix4x4 matWorld;
	};

public:
	/// =====================================
	/// public : methods
	/// =====================================

	TerrainProceduralRenderingPipeline(class GraphicsResourceCollection* _resourceCollection);
	~TerrainProceduralRenderingPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void PreDraw(class EntityComponentSystem* _ecs, class CameraComponent* _camera, class DxCommand* _dxCommand) override;
	void Draw(class EntityComponentSystem* _ecs, const std::vector<IEntity*>& _entities, class CameraComponent* _camera, class DxCommand* _dxCommand) override;

private:
	/// =====================================
	/// private : objects
	/// =====================================

	class GraphicsResourceCollection* pResourceCollection_;
	class DxManager* pDxManager_;

	std::unique_ptr<ComputePipeline> computePipeline_;
	StructuredBuffer<InstanceData> instanceDataAppendBuffer_;

	uint32_t instanceCount_;

};

