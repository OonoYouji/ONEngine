#include "Bloom.h"

#include "ImGuiManager/ImGuiManager.h"

#include "WindowManager/WinApp.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"
#include "GraphicManager/RenderTextureManager/RenderTextureManager.h"

#include "Math/Matrix4x4.h"


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

		void BindToCommandList() {
			pCommandList_->IASetVertexBuffers(0, 1, &vbv_);
			pCommandList_->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_->GetGPUVirtualAddress());
		}

	private:

		ComPtr<ID3D12Resource> viewProjectionBuffer_;
		ComPtr<ID3D12Resource> vertexBuffer_;
		D3D12_VERTEX_BUFFER_VIEW vbv_;

		ID3D12GraphicsCommandList* pCommandList_ = nullptr;
		ONE::DxDescriptor* pDxDescriptor_ = nullptr;

	};

	std::unique_ptr<BloomComponent> gComponent;

} /// namespace



void Bloom::StaticInitialize(ID3D12GraphicsCommandList* commandList, ONE::DxDescriptor* dxDescriptor, uint32_t renderTargetLayerNumber) {
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


	gComponent.reset(new BloomComponent);
	gComponent->Initialize();
	gComponent->pCommandList_ = commandList;
	gComponent->pDxDescriptor_ = dxDescriptor;


}

void Bloom::StaticFinalize() {
	sLuminance_.reset();
	sBloom_.reset();
	sBluer_.reset();
	gComponent.reset();
}

void Bloom::ImGuiDebug(const std::string& treeNodeName) {
	if(ImGui::TreeNodeEx(treeNodeName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("intensity", &bloomMappingData_->intensity, 0.01f);
		ImGui::DragFloat("threshold", &luminanceMappingData_->threshold, 0.001f);
		ImGui::DragFloat2("texSize", &blurMappingData_->texSize.x, 0.0001f);
		ImGui::DragInt("radius", &blurMappingData_->radius, 1);
		ImGui::DragFloat("sigma", &blurMappingData_->sigma, 0.0001f);

		ImGui::TreePop();
	}
}

void Bloom::Initialize() {
	for(auto& renderTex : renderTextures_) {
		renderTex.reset(new RenderTexture());
		renderTex->Initialize(Vec4(0,0,0, 0), gComponent->pCommandList_, gComponent->pDxDescriptor_);
	}

	blurBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(BlurData));
	blurBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&blurMappingData_));
	blurMappingData_->texSize = { 0.001f, 0.001f };
	blurMappingData_->radius = 5;
	blurMappingData_->sigma = 4.0f;

	bloomBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(BloomData));
	bloomBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&bloomMappingData_));
	bloomMappingData_->intensity = 0.7f;
	bloomMappingData_->texSize = {};

	luminanceBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(LuminanceData));
	luminanceBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&luminanceMappingData_));
	luminanceMappingData_->threshold = 0.1f;
}



void Bloom::SetTexSize(const Vec2& texSize) {
	blurMappingData_->texSize = texSize;
}

void Bloom::SetIntensity(float intensity) {
	bloomMappingData_->intensity = intensity;
}

void Bloom::SetRadius(int radius) {
	blurMappingData_->radius = radius;
}


void Bloom::ApplyBloom(RenderTexture* sourceRenderTexture) {
	auto commandList = gComponent->pCommandList_;


	/// 輝度を抽出する
	sLuminance_->SetPipelineState();
	renderTextures_[LUMINANCE_EXTRACTION]->BeginRenderTarget();
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gComponent->BindToCommandList();
	commandList->SetGraphicsRootConstantBufferView(1, luminanceBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, sourceRenderTexture->GetSrvGpuHandle());
	commandList->DrawInstanced(3, 1, 0, 0);
	renderTextures_[LUMINANCE_EXTRACTION]->EndRenderTarget();


	/// blur をかける
	sBluer_->SetPipelineState();
	renderTextures_[BLUR]->BeginRenderTarget();
	gComponent->BindToCommandList();
	commandList->SetGraphicsRootConstantBufferView(1, blurBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, renderTextures_[LUMINANCE_EXTRACTION]->GetSrvGpuHandle());
	commandList->DrawInstanced(3, 1, 0, 0);
	renderTextures_[BLUR]->EndRenderTarget();


	/// bloom エフェクトをかける
	sBloom_->SetPipelineState();
	renderTextures_[BLOOM]->BeginRenderTarget();
	gComponent->BindToCommandList();
	commandList->SetGraphicsRootConstantBufferView(1, bloomBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, sourceRenderTexture->GetSrvGpuHandle());
	commandList->SetGraphicsRootDescriptorTable(3, renderTextures_[BLUR]->GetSrvGpuHandle());
	commandList->DrawInstanced(3, 1, 0, 0);
	renderTextures_[BLOOM]->EndRenderTarget();
}
