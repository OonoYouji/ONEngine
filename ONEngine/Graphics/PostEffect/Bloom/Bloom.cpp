#include "Bloom.h"

#include <WinApp.h>
#include <DxResourceCreator.h>

#include <ImGuiManager.h>
#include <RenderTextureManager.h>

#include <Matrix4x4.h>
#include <Vector2.h>


std::unique_ptr<PipelineState> Bloom::sPipeline_ = nullptr;


namespace {

	class BloomComponent {
		friend class Bloom;
	public:

		BloomComponent() {}
		~BloomComponent() {}

		void Initialize() {

			viewProjectionBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4));

			Mat4* viewProjectionMappingData = nullptr;
			viewProjectionBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&viewProjectionMappingData));
			*viewProjectionMappingData = Mat4::MakeOrthographicMatrix(
				0.0f, 0.0f, float(ONE::WinApp::kWindowSizeX), float(ONE::WinApp::kWindowSizeY), 0.0f, 1000.0f);


			bloomDataBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Bloom::BloomData));
			bloomDataBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&bloomMappingData_));
			bloomMappingData_->intensity = 0.7f;
			bloomMappingData_->threshold = 1.0f;

			struct VertexPosUv {
				Vector4 position;
				Vector2 texcoord;
			};

			VertexPosUv* vertexPosUv = nullptr;
			vertexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(VertexPosUv) * 3);
			vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexPosUv));
			vertexPosUv[0] = { Vec4(0,0,0,1), Vec2(0,0) };
			vertexPosUv[1] = { Vec4(2560,0,0,1), Vec2(2,0) };
			vertexPosUv[2] = { Vec4(0,1440,0,1), Vec2(0,2) };

			vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
			vbv_.SizeInBytes = static_cast<UINT>(sizeof(VertexPosUv) * 3);
			vbv_.StrideInBytes = static_cast<UINT>(sizeof(VertexPosUv));

		}

		void BindFarCommandList() {
			pCommandList->IASetVertexBuffers(0, 1, &vbv_);
			pCommandList->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_->GetGPUVirtualAddress());
			pCommandList->SetGraphicsRootConstantBufferView(1, bloomDataBuffer_->GetGPUVirtualAddress());
		}


	private:

		ComPtr<ID3D12Resource> viewProjectionBuffer_;
		ComPtr<ID3D12Resource> bloomDataBuffer_;
		Bloom::BloomData* bloomMappingData_ = nullptr;

		ComPtr<ID3D12Resource> vertexBuffer_;
		D3D12_VERTEX_BUFFER_VIEW vbv_;

		ID3D12GraphicsCommandList* pCommandList = nullptr;

	};

	std::unique_ptr<BloomComponent> gComponent;

} /// namespace



void Bloom::StaticInitialize(ID3D12GraphicsCommandList* commandList, uint32_t renderTargetLayerNumber) {
	PipelineState::Shader shader{};
	shader.ShaderCompile(
		L"Bloom/Bloom.VS.hlsl", L"vs_6_0",
		L"Bloom/Bloom.PS.hlsl", L"ps_6_0"
	);

	sPipeline_.reset(new PipelineState);
	sPipeline_->SetShader(&shader);
	sPipeline_->SetFillMode(kSolid);
	sPipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	sPipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	sPipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	/// vs cbuffer
	sPipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);

	/// ps cbuffer
	sPipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	sPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	sPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	sPipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	sPipeline_->Initialize();

	/// cbuffer initialize
	gComponent.reset(new BloomComponent);
	gComponent->Initialize();
	gComponent->pCommandList = commandList;

	/// render texture 作成
	RenderTextureManager::CreateRenderTarget("bloom", renderTargetLayerNumber, { 0,0,0,0 });
	RenderTextureManager::SetIsBlending("bloom", true);

}

void Bloom::StaticFinalize() {
	sPipeline_.reset();
	gComponent.reset();
}

void Bloom::ImGuiDebug() {
#ifdef _DEBUG
	if(!ImGui::Begin("Bloom")) {
		ImGui::End();
		return;
	}

	ImGui::DragFloat("intensity", &gComponent->bloomMappingData_->intensity, 0.01f);
	ImGui::DragFloat("threshold", &gComponent->bloomMappingData_->threshold, 0.01f);

	ImGui::End();
#endif // _DEBUG
}



void Bloom::CreateBloomRenderTexture(RenderTexture* sourceRenderTexture) {

	sPipeline_->SetPipelineState();
	auto renderTex = RenderTextureManager::GetRenderTarget("bloom");
	RenderTextureManager::BeginRenderTarget("bloom");

	auto commandList = gComponent->pCommandList;

	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gComponent->BindFarCommandList();
	commandList->SetGraphicsRootDescriptorTable(2, sourceRenderTexture->GetSrvGpuHandle());

	commandList->DrawInstanced(3, 1, 0, 0);

	RenderTextureManager::EndRenderTarget("bloom");

}
