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

	/// @brief compute shader用root param index
	enum CP_ROOT_PARAM : UINT {
		CP_INSNTANCE_DATA,
		CP_SRV_VERTEX_TEXTURE,
		CP_SRV_SPLAT_BLEND_TEXTURE
	};

	/// @brief 
	enum GP_ROOT_PARAM : UINT {
		GP_CBV_VIEW_PROJECTION,
		GP_CBV_TEXTURE_ID,
		GP_SRV_INSNTANCE_DATA,
		GP_SRV_TEXTURES
	};

	struct InstanceData {
		Matrix4x4 matWorld;
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

	std::unique_ptr<ComputePipeline> computePipeline_;
	StructuredBuffer<InstanceData> instanceDataAppendBuffer_;
	ConstantBuffer<TextureId> textureIdBuffer_;

	uint32_t instanceCount_;
	bool isFirstPreDraw_ = true;

};

