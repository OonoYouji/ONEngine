#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

/// //////////////////////////////////////////////////
/// レンダリング結果を画面に表示するShader
/// //////////////////////////////////////////////////
class CopyImageRenderingPipeline : public IRenderingPipeline {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	CopyImageRenderingPipeline(class AssetCollection* _assetCollection);
	~CopyImageRenderingPipeline() = default;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxm) override;
	void Draw(class ECSGroup* _ecs, const std::vector<GameEntity*>& _entities, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class AssetCollection* pAssetCollection_ = nullptr;
};

