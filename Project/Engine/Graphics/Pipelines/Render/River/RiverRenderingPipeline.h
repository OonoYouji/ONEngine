#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

namespace ONEngine::Asset {
class AssetCollection;
}

/// /////////////////////////////////////////////////
/// 川の描画pipeline
/// /////////////////////////////////////////////////
namespace ONEngine {

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

	RiverRenderingPipeline(Asset::AssetCollection* _assetCollection);
	~RiverRenderingPipeline() override;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Draw(class ECSGroup* _ecs, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// =================================================
	/// private : objects
	/// =================================================

	Asset::AssetCollection* pAssetCollection_ = nullptr;
};

} /// ONEngine
