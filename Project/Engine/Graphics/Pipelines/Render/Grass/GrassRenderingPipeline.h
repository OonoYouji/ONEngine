#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/ByteAddressBuffer.h"

#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Grass/GrassField.h"

/// /////////////////////////////////////////////////
/// 草を描画するパイプライン
/// /////////////////////////////////////////////////
namespace ONEngine {

class GrassRenderingPipeline : public IRenderingPipeline {

	enum ROOT_PARAM {
		CONSTANT_32BIT_DATA,
		CBV_VIEW_PROJECTION,
		CBV_MATERIAL,
		ROOT_PARAM_BLADES,
		SRV_START_INDEX,
		ROOT_PARAM_TIME,
		SRV_TEXTURES,
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GrassRenderingPipeline(class AssetCollection* _assetCollection);
	~GrassRenderingPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void PreDraw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxManager* pDxManager_ = nullptr;
	class AssetCollection* pAssetCollection_ = nullptr;
};


} /// ONEngine
