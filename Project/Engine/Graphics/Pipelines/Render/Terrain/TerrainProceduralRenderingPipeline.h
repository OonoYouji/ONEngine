#pragma once

/// std
#include <cstdint>

/// engine
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Core/Utility/Math/Matrix4x4.h"
#include "Engine/Graphics/Shader/ComputePipeline.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"

/// /////////////////////////////////////////////////
/// 地形に対してのプロシージャルレンダリングパイプライン
/// /////////////////////////////////////////////////
class TerrainProceduralRenderingPipeline : public IRenderingPipeline {

	/// @brief 配置用Shaderのルートパラメータ
	enum ARR_ROOT_PARAM : UINT {
		ARR_DATA,
		ARR_INSNTANCE_DATA,
		ARR_SRV_VERTEX_TEXTURE,
		ARR_SRV_SPLAT_BLEND_TEXTURE,
		ARR_SRV_TREE_ARRANGEMENT_TEXTURE,
	};

	/// @brief カリング用Shaderのルートパラメータ
	enum CALL_ROOT_PATAM : UINT {
		CULL_CBV_VIEW_PROJECTION,
		CULL_CBV_MAX_INSTANCE_COUNT,
		CULL_SRV_INSNTANCE_DATA,
		CULL_UAV_RENDERING_INSTANCE,
	};

	/// @brief 
	enum GP_ROOT_PARAM : UINT {
		GP_CBV_VIEW_PROJECTION,
		GP_CBV_TEXTURE_ID,
		GP_SRV_INSNTANCE_DATA,
		GP_SRV_RENDERING_INSTANCE,
		GP_SRV_TEXTURES
	};

	struct InstanceData {
		Matrix4x4 matWorld;
		Vector4 minBounds;
		Vector4 maxBounds;
	};

	struct RenderingInstance {
		uint32_t id;
	};

	struct TextureId {
		uint32_t value;
	};

public:
	/// =====================================
	/// public : methods
	/// =====================================

	TerrainProceduralRenderingPipeline(class AssetCollection* _assetCollection);
	~TerrainProceduralRenderingPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void PreDraw(class ECSGroup* _ecs, class CameraComponent* _camera, class DxCommand* _dxCommand) override;
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, class DxCommand* _dxCommand) override;

private:
	/// =====================================
	/// private : objects
	/// =====================================

	class AssetCollection* pAssetCollection_;
	class DxManager* pDxManager_;

	std::unique_ptr<ComputePipeline> arrangementPipeline_;
	std::unique_ptr<ComputePipeline> cullingPipeline_;

	StructuredBuffer<InstanceData> instanceDataAppendBuffer_;
	StructuredBuffer<RenderingInstance> renderingInstanceAppendBuffer_;
	ConstantBuffer<TextureId> textureIdBuffer_;
	ConstantBuffer<float> dataBuffer_;
	ConstantBuffer<int> maxInstanceCountBuffer_;

	uint32_t instanceCount_;
	uint32_t drawInstanceCount_;
	bool isFirstPreDraw_ = true;

};

