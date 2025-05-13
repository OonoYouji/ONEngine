#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

/// ///////////////////////////////////////////////////////////////////////////////////////////
/// 天球のレンダリングパイプライン
/// ///////////////////////////////////////////////////////////////////////////////////////////
class SkyboxRenderingPipeline : IRenderingPipeline {
public:

	SkyboxRenderingPipeline();
	~SkyboxRenderingPipeline();

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(DxCommand* _dxCommand, class EntityComponentSystem* _pEntityComponentSystem, class Camera* _camera) override;

private:
};

