#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/ByteAddressBuffer.h"

#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Grass/GrassField.h"

namespace ONEngine {
class DxManager;
class ECSGroup;
class CameraComponent;
}

namespace ONEngine::Asset {
class AssetCollection;
}


namespace ONEngine {

/// /////////////////////////////////////////////////
/// 草を描画するパイプライン
/// /////////////////////////////////////////////////
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

	GrassRenderingPipeline(Asset::AssetCollection* _assetCollection);
	~GrassRenderingPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) override;
	void PreDraw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) override;
	void Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	DxManager* pDxManager_ = nullptr;
	Asset::AssetCollection* pAssetCollection_ = nullptr;
};


} /// ONEngine
