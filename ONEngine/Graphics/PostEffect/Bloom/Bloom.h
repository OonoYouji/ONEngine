#pragma once

#include <memory>

#include <PipelineState.h>

class RenderTexture;

class Bloom final {
public:

	static void StaticInitialize();
	static void StaticFinalize();

	static std::unique_ptr<RenderTexture> CreateBroomRenderTexture(
		RenderTexture* sourceRenderTexture
	);

private:

	static std::unique_ptr<PipelineState> sPipeline_;

};