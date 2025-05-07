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
	void Draw(DxCommand* _dxCommand, class EntityComponentSystem* _pEntityComponentSystem, class Camera* _camera) override;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class GraphicsResourceCollection* pResourceCollection_ = nullptr;
};

