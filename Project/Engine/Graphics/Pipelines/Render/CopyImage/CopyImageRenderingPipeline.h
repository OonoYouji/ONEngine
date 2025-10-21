#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

/// //////////////////////////////////////////////////
/// レンダー結果をコピーするためのパイプライン
/// //////////////////////////////////////////////////
class CopyImageRenderingPipeline : public IRenderingPipeline {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	CopyImageRenderingPipeline(class AssetCollection* _graphicsResourceCollection);
	~CopyImageRenderingPipeline() = default;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(class ECSGroup* _ecs, const std::vector<GameEntity*>& _entities, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class AssetCollection* pGrc_ = nullptr;
};

