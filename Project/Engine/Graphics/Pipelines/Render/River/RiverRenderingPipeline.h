#pragma once

/// engine
#include "../../Interface/IRenderingPipeline.h"

/// ////////////////////////////////////
/// 川の描画pipeline
/// ////////////////////////////////////
class RiverRenderingPipeline : public IRenderingPipeline {

	enum ROOT_PARAM {
		CBV_VIEW_PROJECTION,
		CBV_MATERIAL,
		SRV_TEXTURE,
	};

public:

	RiverRenderingPipeline(class GraphicsResourceCollection* _grc);
	~RiverRenderingPipeline() override;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(class ECSGroup* _ecs, const std::vector<GameEntity*>& _entities, class CameraComponent* _camera, DxCommand* _dxCommand) override;

private:
	GraphicsResourceCollection* pGRC_ = nullptr;
};