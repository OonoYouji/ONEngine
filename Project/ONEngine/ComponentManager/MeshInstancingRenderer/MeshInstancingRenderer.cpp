#include "MeshInstancingRenderer.h"

/// std
#include <memory>
#include <vector>

/// direct x
#include <d3d12.h>
#include <wrl/client.h>

/// engine
#include <imgui.h>
#include "GraphicManager/PipelineState/PipelineState.h"
#include "GraphicManager/Light/DirectionalLight.h"

/// using namespace
using namespace Microsoft::WRL;



namespace {

	struct RenderElement {
		Transform* pTransform = nullptr;
	};

	/// ===================================================
	/// ↓ レンダリングパイプライン
	/// ===================================================
	class RenderingPipeline {
		friend class MeshInstancingRenderer;
	public:
		RenderingPipeline() {}
		~RenderingPipeline() {}

		void Initialize(
			ID3D12GraphicsCommandList* _commandList
		);

		void Draw();

	private:

		/// data
		std::vector<RenderElement> renderElementArray_;

		/// pso
		std::unique_ptr<PipelineState> pipeline_ = nullptr;
		PipelineState::Shader shader_;

		/// other pointer
		DirectionalLight*          pDirectionalLight_ = nullptr;
		ID3D12GraphicsCommandList* pCommnadList_      = nullptr;

	};


	/// ===================================================
	/// ↓ methods
	/// ===================================================

	void RenderingPipeline::Initialize(ID3D12GraphicsCommandList* _commandList) {


		/// set pointer
		pCommnadList_ = _commandList;
		assert(pCommnadList_);


		/// shader compile
		shader_.ShaderCompile(
			L"MeshInstancingRenderer/MeshInstancingRenderer.VS.hlsl", L"vs_6_0",
			L"MeshInstancingRenderer/MeshInstancingRenderer.PS.hlsl", L"ps_6_0"
		);

		/// pso create
		pipeline_.reset(new PipelineState);
		pipeline_->SetFillMode(kSolid);
		pipeline_->SetShader(&shader_);
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);	///- viewProjection
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1);	///- transform
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);	///- material
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);	///- directional light

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

		pipeline_->Initialize();
	}

	void RenderingPipeline::Draw() {
		/// default setting
		pipeline_->SetPipelineState();
		pCommnadList_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDirectionalLight_->BindToCommandList(3, pCommnadList_);
	}



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

void MeshInstancingRenderer::SInitialize(ID3D12GraphicsCommandList* _commandList) {
	gPipeline.reset(new RenderingPipeline);
	gPipeline->Initialize(_commandList);
}

void MeshInstancingRenderer::SFinalize() {
	gPipeline.reset();
}


void MeshInstancingRenderer::SetDirectionalLight(DirectionalLight* _directionalLight) {
	gPipeline->pDirectionalLight_ = _directionalLight;
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
