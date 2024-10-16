#include "MeshInstancingRenderer.h"

/// std
#include <memory>

/// engine
#include <imgui.h>
#include "GraphicManager/PipelineState/PipelineState.h"




namespace {

	/// ===================================================
	/// ↓ レンダリングパイプライン
	/// ===================================================
	class RenderingPipeline {
		friend class MeshInstancingRenderer;
	public:
		RenderingPipeline() {}
		~RenderingPipeline() {}

		void Initialize();

		void PreDraw();
		void PostDraw();

	private:

		std::unique_ptr<PipelineState> pipeline_ = nullptr;
		PipelineState::Shader shader_;

	};


	/// ===================================================
	/// ↓ methods
	/// ===================================================

	void RenderingPipeline::Initialize() {
		shader_.ShaderCompile(
			L"MeshInstancingRenderer/MeshInstacingRenderer.VS.hlsl", L"vs_6_0",
			L"MeshInstancingRenderer/MeshInstacingRenderer.PS.hlsl", L"ps_6_0"
		);
	}

	void RenderingPipeline::PreDraw() {}

	void RenderingPipeline::PostDraw() {}



	/// ===================================================
	/// ↓ objects
	/// ===================================================

	std::unique_ptr<RenderingPipeline> gPipeline = nullptr;

} //// namespace



MeshInstancingRenderer::MeshInstancingRenderer(uint32_t maxInstanceCount) : kMaxInstanceCount_(maxInstanceCount) {

}

/// ===================================================
/// static methods
/// ===================================================

void MeshInstancingRenderer::SInitialize() {
	gPipeline.reset(new RenderingPipeline);
	gPipeline->Initialize();
}

void MeshInstancingRenderer::SFinalize() {
	gPipeline.reset();
}

void MeshInstancingRenderer::PreDraw() {
	gPipeline->PreDraw();
}

void MeshInstancingRenderer::PostDraw() {
	gPipeline->PostDraw();
}


/// ===================================================
/// normal methods
/// ===================================================

void MeshInstancingRenderer::Initialize() {}

void MeshInstancingRenderer::Update() {}

void MeshInstancingRenderer::Draw() {

}

void MeshInstancingRenderer::Debug() {

}
