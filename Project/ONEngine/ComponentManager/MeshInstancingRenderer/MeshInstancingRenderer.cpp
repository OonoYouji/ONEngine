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
#include "GraphicManager/ModelManager/ModelManager.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDevice.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptorHeap.h"

/// objects
#include "Objects/Camera/Manager/CameraManager.h"

/// using namespace
using namespace Microsoft::WRL;
using namespace ONE;


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

		void Draw(D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle, Model* useModel, uint32_t instanceCount);

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
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);	///- material
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);	///- directional light

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 1); /// transform

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0); /// texture
		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

		pipeline_->Initialize();
	}

	void RenderingPipeline::Draw(D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle, Model* useModel, uint32_t instanceCount) {

		/// other pointer
		ID3D12Resource* viewBuffer = CameraManager::GetInstance()->GetMainCamera()->GetViewBuffer();

		/// default setting
		pipeline_->SetPipelineState();
		pCommnadList_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		/// buffer setting
		pDirectionalLight_->BindToCommandList(2, pCommnadList_);
		pCommnadList_->SetGraphicsRootConstantBufferView(0, viewBuffer->GetGPUVirtualAddress());
		pCommnadList_->SetGraphicsRootDescriptorTable(3, gpuHandle);

		for(size_t i = 0; i < useModel->GetMeshes().size(); ++i) {

			Mesh&     mesh     = useModel->GetMeshes()[i];
			Material& material = useModel->GetMaterials().front();

			material.BindMaterial(pCommnadList_, 1);
			material.BindTexture(pCommnadList_, 4);
			mesh.Draw(pCommnadList_, false);

			UINT meshIndex = static_cast<UINT>(mesh.GetIndices().size());
			pCommnadList_->DrawIndexedInstanced(meshIndex, instanceCount, 0, 0, 0);
		}
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


	DxDescriptorHeap<HeapType::CBV_SRV_UAV>* pSRVDescriptorHeap = ONEngine::GetDxCommon()->GetSRVDescriptorHeap();

	uint32_t srvDescriptorIndex = pSRVDescriptorHeap->Allocate();
	cpuHandle_ = pSRVDescriptorHeap->GetCPUDescriptorHandel(srvDescriptorIndex);
	gpuHandle_ = pSRVDescriptorHeap->GetGPUDescriptorHandel(srvDescriptorIndex);

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

	std::vector<Mat4> matTransformArray{};
	for(auto& transform : transformArray_) {
		transform->SetName(std::format("Transform##{:p}", reinterpret_cast<void*>(&transform)));

		matTransformArray.push_back(transform->matTransform);
	}

	std::memcpy(mappingData_, matTransformArray.data(), matTransformArray.size() * sizeof(Mat4));

	gPipeline->Draw(gpuHandle_, model_, static_cast<uint32_t>(transformArray_.size()));
}

void MeshInstancingRenderer::Debug() {
	if(ImGui::TreeNodeEx(GetName().c_str())) {

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

void MeshInstancingRenderer::SetModel(const std::string& filePath) {
	model_ = ModelManager::Load(filePath);
}

void MeshInstancingRenderer::SetModel(Model* model) {
	model_ = model;
}
