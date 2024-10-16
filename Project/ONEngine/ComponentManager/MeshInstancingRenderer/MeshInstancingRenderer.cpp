#include "MeshInstancingRenderer.h"

/// std
#include <memory>
#include <vector>

/// direct x
#include <d3d12.h>
#include <wrl/client.h>

/// externals
#include <imgui.h>

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/PipelineState/PipelineState.h"
#include "GraphicManager/Light/DirectionalLight.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptor.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDevice.h"
#include "GraphicManager/ModelManager/ModelManager.h"


/// using namespace
using namespace Microsoft::WRL;



namespace {

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

void MeshInstancingRenderer::Initialize() {
	assert(kMaxInstanceCount_ > 0); /// max instanceが0以下で assertion...
	

	/// buffer create
	transformArrayBuffer_ = ONE::DxResourceCreator::CreateResource(
		sizeof(Mat4) * kMaxInstanceCount_);

	/// desc setting
	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format                     = DXGI_FORMAT_UNKNOWN;
	desc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement        = 0;
	desc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_NONE;
	desc.Buffer.NumElements         = static_cast<UINT>(kMaxInstanceCount_);
	desc.Buffer.StructureByteStride = sizeof(Mat4);

	/// cpu, gpu handle initialize
	auto dxDescriptor = ONEngine::GetDxCommon()->GetDxDescriptor();
	cpuHandle_ = dxDescriptor->GetSrvCpuHandle();
	gpuHandle_ = dxDescriptor->GetSrvGpuHandle();
	dxDescriptor->AddSrvUsedCount();

	/// resource create
	auto dxDevice = ONEngine::GetDxCommon()->GetDxDevice();
	dxDevice->GetDevice()->CreateShaderResourceView(transformArrayBuffer_.Get(), &desc, cpuHandle_);

	/// mapping data bind
	transformArrayBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappingData_));


	/// model setting
	model_ = ModelManager::Load("Sphere");

}

void MeshInstancingRenderer::Update() {}

void MeshInstancingRenderer::Draw() {

	/// 描画するインスタンスが0なら描画しない
	if(transformArray_.empty()) {
		return;
	}

	gPipeline->Draw();
}

void MeshInstancingRenderer::Debug() {
	if(ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::BeginChild("space", ImVec2(0.0f, 360.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
		ImGui::SeparatorText("transform debug draw");

		for(size_t i = 0; i < transformArray_.size(); ++i) {
			transformArray_[i]->Debug();
		}

		ImGui::EndChild();

		ImGui::TreePop();
	}

}

void MeshInstancingRenderer::AddTransform(Transform* transform) {
	transformArray_.push_back(transform);
}

void MeshInstancingRenderer::SetTransformArray(const std::vector<Transform*>& transformArray) {
	transformArray_ = transformArray;
}

void MeshInstancingRenderer::ResetTransformArray() {
	transformArray_.clear();
}
