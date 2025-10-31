#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

/// /////////////////////////////////////////////////
/// 川の描画pipeline
/// /////////////////////////////////////////////////
class RiverRenderingPipeline : public IRenderingPipeline {

	enum ROOT_PARAM {
		CBV_VIEW_PROJECTION,
		CBV_MATERIAL,
		SRV_TEXTURE,
	};

public:
	/// =================================================
	/// public : methods
	/// =================================================

	RiverRenderingPipeline(class AssetCollection* _assetCollection);
	~RiverRenderingPipeline() override;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// =================================================
	/// private : objects
	/// =================================================

	AssetCollection* pAssetCollection_ = nullptr;
};