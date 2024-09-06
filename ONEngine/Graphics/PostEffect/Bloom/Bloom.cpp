#include "Bloom.h"

#include <WinApp.h>
#include <DxResourceCreator.h>

#include <ImGuiManager.h>
#include <RenderTextureManager.h>

#include <Matrix4x4.h>


std::unique_ptr<PipelineState> Bloom::sBloom_ = nullptr;
std::unique_ptr<PipelineState> Bloom::sBluer_ = nullptr;
std::unique_ptr<PipelineState> Bloom::sLuminance_ = nullptr;


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

			blurBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Bloom::BlurData));
			blurBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&blurMappingData_));
			blurMappingData_->texSize = { 0.001f, 0.001f };

			bloomDataBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Bloom::BloomData));
			bloomDataBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&bloomMappingData_));
			bloomMappingData_->intensity = 0.7f;
			bloomMappingData_->texSize = {};

			luminanceBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Bloom::LuminanceData));
			luminanceBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&luminanceMappingData_));
			luminanceMappingData_->threshold = 0.1f;

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

		void BloomBindFarCommandList() {
			pCommandList->IASetVertexBuffers(0, 1, &vbv_);
			pCommandList->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_->GetGPUVirtualAddress());
			pCommandList->SetGraphicsRootConstantBufferView(1, bloomDataBuffer_->GetGPUVirtualAddress());
		}

		void BlurBindFarCommandList() {
			pCommandList->IASetVertexBuffers(0, 1, &vbv_);
			pCommandList->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_->GetGPUVirtualAddress());
			pCommandList->SetGraphicsRootConstantBufferView(1, blurBuffer_->GetGPUVirtualAddress());
		}

		void LuminanceBindFarCommandList() {
			pCommandList->IASetVertexBuffers(0, 1, &vbv_);
			pCommandList->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_->GetGPUVirtualAddress());
			pCommandList->SetGraphicsRootConstantBufferView(1, luminanceBuffer_->GetGPUVirtualAddress());
		}


	private:

		ComPtr<ID3D12Resource> bloomDataBuffer_;
		Bloom::BloomData* bloomMappingData_ = nullptr;

		ComPtr<ID3D12Resource> blurBuffer_;
		Bloom::BlurData* blurMappingData_ = nullptr;

		ComPtr<ID3D12Resource> luminanceBuffer_;
		Bloom::LuminanceData* luminanceMappingData_ = nullptr;


		ComPtr<ID3D12Resource> viewProjectionBuffer_;
		ComPtr<ID3D12Resource> vertexBuffer_;
		D3D12_VERTEX_BUFFER_VIEW vbv_;

		ID3D12GraphicsCommandList* pCommandList = nullptr;

	};

	std::unique_ptr<BloomComponent> gComponent;

} /// namespace



void Bloom::StaticInitialize(ID3D12GraphicsCommandList* commandList, uint32_t renderTargetLayerNumber) {
	PipelineState::Shader bloomShader{};
	bloomShader.ShaderCompile(
		L"Bloom/Bloom.VS.hlsl", L"vs_6_0",
		L"Bloom/Bloom.PS.hlsl", L"ps_6_0"
	);

	sBloom_.reset(new PipelineState);
	sBloom_->SetShader(&bloomShader);
	sBloom_->SetFillMode(kSolid);
	sBloom_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	sBloom_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	sBloom_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	/// vs cbuffer
	sBloom_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);

	/// ps cbuffer
	sBloom_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	sBloom_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	sBloom_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	sBloom_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	sBloom_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);
	sBloom_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	sBloom_->Initialize();


	PipelineState::Shader blurShader{};
	blurShader.ShaderCompile(
		L"Blur/Blur.VS.hlsl", L"vs_6_0",
		L"Blur/Blur.PS.hlsl", L"ps_6_0"
	);

	sBluer_.reset(new PipelineState);
	sBluer_->SetShader(&blurShader);
	sBluer_->SetFillMode(kSolid);
	sBluer_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	sBluer_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	sBluer_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	/// vs cbuffer
	sBluer_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);

	/// ps cbuffer
	sBluer_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	sBluer_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	sBluer_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	sBluer_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	sBluer_->Initialize();


	PipelineState::Shader luminanceShader{};
	luminanceShader.ShaderCompile(
		L"LuminanceExtraction/LuminanceExtraction.VS.hlsl", L"vs_6_0",
		L"LuminanceExtraction/LuminanceExtraction.PS.hlsl", L"ps_6_0"
	);

	sLuminance_.reset(new PipelineState);
	sLuminance_->SetShader(&luminanceShader);
	sLuminance_->SetFillMode(kSolid);
	sLuminance_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	sLuminance_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	sLuminance_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	/// vs cbuffer
	sLuminance_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);

	/// ps cbuffer
	sLuminance_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	sLuminance_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	sLuminance_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	sLuminance_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	sLuminance_->Initialize();




	/// cbuffer initialize
	gComponent.reset(new BloomComponent);
	gComponent->Initialize();
	gComponent->pCommandList = commandList;

	/// render texture 作成
	RenderTextureManager::CreateRenderTarget("bloom", renderTargetLayerNumber - 1, { 0,0,0,0 });
	RenderTextureManager::CreateRenderTarget("blur", renderTargetLayerNumber, { 0,0,0,0 });
	RenderTextureManager::CreateRenderTarget("luminance", renderTargetLayerNumber + 1, { 0,0,0,0 });

	RenderTextureManager::SetIsBlending("bloom", true);
	RenderTextureManager::SetIsBlending("blur", false);
	RenderTextureManager::SetIsBlending("luminance", false);

}

void Bloom::StaticFinalize() {
	sLuminance_.reset();
	sBloom_.reset();
	sBluer_.reset();
	gComponent.reset();
}

void Bloom::ImGuiDebug() {
#ifdef _DEBUG
	if(!ImGui::Begin("Bloom")) {
		ImGui::End();
		return;
	}

	ImGui::DragFloat("intensity", &gComponent->bloomMappingData_->intensity, 0.01f);
	ImGui::DragFloat("threshold", &gComponent->luminanceMappingData_->threshold, 0.001f);
	ImGui::DragFloat2("texSize", &gComponent->blurMappingData_->texSize.x, 0.0001f);

	ImGui::End();
#endif // _DEBUG
}



void Bloom::CreateBloomRenderTexture(RenderTexture* sourceRenderTexture) {

	auto commandList = gComponent->pCommandList;

	sLuminance_->SetPipelineState();
	RenderTextureManager::BeginRenderTarget("luminance");
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gComponent->LuminanceBindFarCommandList();
	commandList->SetGraphicsRootDescriptorTable(2, sourceRenderTexture->GetSrvGpuHandle());
	commandList->DrawInstanced(3, 1, 0, 0);
	RenderTextureManager::EndRenderTarget("luminance");


	auto luminanceRenderTex = RenderTextureManager::GetRenderTarget("luminance");
	sBluer_->SetPipelineState();
	RenderTextureManager::BeginRenderTarget("blur");
	gComponent->BlurBindFarCommandList();
	commandList->SetGraphicsRootDescriptorTable(2, luminanceRenderTex->GetSrvGpuHandle());
	commandList->DrawInstanced(3, 1, 0, 0);
	RenderTextureManager::EndRenderTarget("blur");


	auto blurRenderTex = RenderTextureManager::GetRenderTarget("blur");
	sBloom_->SetPipelineState();
	RenderTextureManager::BeginRenderTarget("bloom");
	gComponent->BloomBindFarCommandList();
	commandList->SetGraphicsRootDescriptorTable(2, sourceRenderTexture->GetSrvGpuHandle());
	commandList->SetGraphicsRootDescriptorTable(3, blurRenderTex->GetSrvGpuHandle());
	commandList->DrawInstanced(3, 1, 0, 0);
	RenderTextureManager::EndRenderTarget("bloom");


}
