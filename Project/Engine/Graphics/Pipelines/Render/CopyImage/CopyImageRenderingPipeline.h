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

	CopyImageRenderingPipeline(class GraphicsResourceCollection* _graphicsResourceCollection);
	~CopyImageRenderingPipeline() = default;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(const std::vector<IEntity*>& _entities, class Camera* _camera, DxCommand* _dxCommand) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class GraphicsResourceCollection* pResourceCollection_ = nullptr;
};

