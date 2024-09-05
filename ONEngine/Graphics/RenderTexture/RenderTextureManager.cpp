#include "RenderTextureManager.h"

#include <algorithm>

#include <WinApp.h>
#include <DxResourceCreator.h>
#include <Matrix4x4.h>
#include <Vector2.h>


RenderTextureManager RenderTextureManager::sInstance_;


void RenderTextureManager::Initialize(ID3D12GraphicsCommandList* commandList, ONE::DxDescriptor* descriptor) {
	pCommandList_ = commandList;
	pDxDescriptor_ = descriptor;

	/// ===================================================
	/// pipeline state initialize
	/// ===================================================
	PipelineState::Shader shader{};
	shader.ShaderCompile(
		L"BlendRenderTexture/BlendRenderTexture.VS.hlsl", L"vs_6_0",
		L"BlendRenderTexture/BlendRenderTexture.PS.hlsl", L"ps_6_0"
	);

	pipeline_.reset(new PipelineState);

	pipeline_->SetShader(&shader);
	pipeline_->SetFillMode(kSolid);
	pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	/// vs cbuffer
	pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);

	/// ps cbuffer
	pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);
	pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	pipeline_->Initialize();


	/// ===================================================
	/// view projection buffer initialize
	/// ===================================================
	Mat4* viewProjectionData = nullptr;
	Matrix4x4 projectionMatrix = Mat4::MakeOrthographicMatrix(
		0.0f, 0.0f, float(ONE::WinApp::kWindowSizeX), float(ONE::WinApp::kWindowSizeY), 0.0f, 1000.0f);

	viewProjectionBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4));
	viewProjectionBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&viewProjectionData));
	*viewProjectionData = projectionMatrix;


	/// ===================================================
	/// vbv ivb
	/// ===================================================

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



	for(auto& output : outputs_) {
		output.reset(new RenderTexture);
		output->Initialize({ 0,0,0,1 }, pCommandList_, pDxDescriptor_);
	}

}



void RenderTextureManager::Finalize() {
	renderTextures_.clear();
	for(auto& output : outputs_) {
		output.reset();
	}

	vertexBuffer_.Reset();
	viewProjectionBuffer_.Reset();
	pipeline_.reset();
}

void RenderTextureManager::End() {

	std::vector<uint32_t> values;
	for(const auto& pair : layerNumbers_) {
		values.push_back(pair.second);
	}

	std::sort(values.begin(), values.end());

	auto frontItr = values.begin();
	auto nextItr = std::next(frontItr);
	renderTextures_[(*frontItr)]->BlendRenderTexture(renderTextures_[(*nextItr)].get(), outputs_[outputsIndex_].get());

	for(auto itr = nextItr; itr != values.end(); ++itr) {
		auto next = std::next(itr);
		if(next == values.end()) { break; }

		if(outputsIndex_ == 1) {
			outputsIndex_ = 0;
		} else {
			outputsIndex_ = 1;
		}

		outputs_[outputsIndex_ ? 0 : 1]->BlendRenderTexture(renderTextures_[(*next)].get(), outputs_[outputsIndex_].get());
	}
}

void RenderTextureManager::BindForCommandList() {
	pipeline_->SetPipelineState();
	pCommandList_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList_->IASetVertexBuffers(0, 1, &vbv_);

	/// view projection
	pCommandList_->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_->GetGPUVirtualAddress());
}

void RenderTextureManager::SetRenderTarget(const std::string& name) {
	sInstance_.renderTextures_[sInstance_.layerNumbers_.at(name)]->SetRenderTarget();
}


void RenderTextureManager::CreateRenderTarget(const std::string& name, uint32_t layerNumber) {
	std::unique_ptr<RenderTexture> newRenderTex;
	newRenderTex.reset(new RenderTexture());
	newRenderTex->Initialize({ 0,0,0,0 }, sInstance_.pCommandList_, sInstance_.pDxDescriptor_);

	sInstance_.renderTextures_.push_back(std::move(newRenderTex));
	sInstance_.layerNumbers_[name] = layerNumber;
}
