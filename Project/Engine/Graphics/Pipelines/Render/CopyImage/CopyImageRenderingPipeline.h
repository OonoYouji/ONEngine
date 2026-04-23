#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"


namespace ONEngine {
class DxManager;
class ECSGroup;
class CameraComponent;
}

namespace ONEngine::Asset {
class AssetCollection;
}



/// //////////////////////////////////////////////////
/// レンダリング結果を画面に表示するShader
/// //////////////////////////////////////////////////
namespace ONEngine {

class CopyImageRenderingPipeline : public IRenderingPipeline {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	CopyImageRenderingPipeline(Asset::AssetCollection* _assetCollection);
	~CopyImageRenderingPipeline() = default;

	void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) override;
	void Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Asset::AssetCollection* pAssetCollection_ = nullptr;
};


} /// ONEngine
