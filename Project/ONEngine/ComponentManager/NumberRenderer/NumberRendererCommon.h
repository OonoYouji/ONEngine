#pragma once

/// std
#include <memory>

/// engine
#include "GraphicManager/PipelineState/PipelineState.h"


class NumberRendererCommon final {
	NumberRendererCommon()  = default;
	~NumberRendererCommon() = default;
public:

	static NumberRendererCommon* GetInstance() {
		static NumberRendererCommon instance;
		return &instance;
	}


	void Initialize();
	void Finalize();

	void PreDraw();
	void PostDraw();

private:

	std::unique_ptr<PipelineState> pipelineState_ = nullptr;
	PipelineState::Shader          shader_        = {};

};