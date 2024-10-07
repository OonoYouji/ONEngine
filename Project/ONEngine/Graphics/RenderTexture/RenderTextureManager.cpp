#include "RenderTextureManager.h"

#include <algorithm>
#include <array>

#include <WinApp.h>
#include <DxResourceCreator.h>
#include <DxBarrierCreator.h>
#include <ImGuiManager.h>

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



	finalRenderTex_.reset(new RenderTexture);
	finalRenderTex_->Initialize({ 0,0,0,0 }, pCommandList_, pDxDescriptor_);

	for(auto& output : intermediateTextures_) {
		output.reset(new RenderTexture);
		output->Initialize({ 0,0,0,0 }, pCommandList_, pDxDescriptor_);
	}

}



void RenderTextureManager::Finalize() {

	renderTextures_.clear();
	finalRenderTex_.reset();
	for(auto& output : intermediateTextures_) {
		output.reset();
	}

	vertexBuffer_.Reset();
	viewProjectionBuffer_.Reset();
	pipeline_.reset();
}

void RenderTextureManager::ImGuiDebug() {
#ifdef _DEBUG
	if(!ImGui::Begin("RenderTextureMaanger")) {
		ImGui::End();
		return;
	}

	for(auto& tex : renderTexDatas_) {
		ImGui::Checkbox((tex.first + std::string("  isBlending")).c_str(), &tex.second.isBlending);
	}

	ImGui::End();

#endif // _DEBUG
}

void RenderTextureManager::EndFrame() {

	int blendingNum = 0;
	for(auto& tex : renderTexDatas_) {
		if(tex.second.isBlending) {
			blendingNum++;
		}
	}

	if(blendingNum < 2) {
		return;
	}

	std::vector<uint32_t> values;
	for(const auto& pair : renderTexDatas_) {
		if(pair.second.isBlending) {
			values.push_back(pair.second.layerNum);
		}
	}

	std::sort(values.begin(), values.end());

	std::vector<RenderTexture*> renderTexs;
	for(auto value : values) {
		renderTexs.push_back(renderTextures_[value].get());
	}

	CreateBlendRenderTexture(renderTexs, finalRenderTex_.get());


}

void RenderTextureManager::BindForCommandList() {
	pipeline_->SetPipelineState();
	pCommandList_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList_->IASetVertexBuffers(0, 1, &vbv_);

	/// view projection
	pCommandList_->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_->GetGPUVirtualAddress());
}



void RenderTextureManager::CreateBlendRenderTexture(
	std::vector<RenderTexture*> renderTextures, RenderTexture* outputRenderTexture) {

	if(renderTextures.size() < 2) {
		assert(false);
		return;
	}

	/// output に render するための中間リソース
	uint8_t outputSideIndex = 0;	/// output先のtexIndex
	std::array<RenderTexture*, 2> intermediates;
	intermediates[0] = sInstance_.intermediateTextures_[0].get();
	intermediates[1] = sInstance_.intermediateTextures_[1].get();


	/// ---------------------------------------------- /// 
	///		intermediate を使って render textureを重ねる
	/// ---------------------------------------------- /// 

	/// barrier を変える
	for(auto& renderTex : renderTextures) {
		ONE::DxBarrierCreator::CreateBarrier(
			renderTex->GetRenderTexResource(),
			renderTex->currentResourceState,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
		renderTex->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	}

	renderTextures[0]->BlendRenderTexture(renderTextures[1], intermediates[outputSideIndex]);

	ONE::DxBarrierCreator::CreateBarrier(
		intermediates[outputSideIndex]->GetRenderTexResource(),
		intermediates[outputSideIndex]->currentResourceState,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	intermediates[outputSideIndex]->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	outputSideIndex = 1;

	for(auto frontTex = (renderTextures.begin() + 2); frontTex != renderTextures.end(); ++frontTex) {

		/// back と outputへのポインタ
		RenderTexture* backTex = intermediates[1 - outputSideIndex];
		RenderTexture* outputTex = intermediates[outputSideIndex];

		/// barrier の設定を変える
		ONE::DxBarrierCreator::CreateBarrier(backTex->GetRenderTexResource(), backTex->currentResourceState, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		ONE::DxBarrierCreator::CreateBarrier(outputTex->GetRenderTexResource(), outputTex->currentResourceState, D3D12_RESOURCE_STATE_RENDER_TARGET);
		backTex->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		outputTex->currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

		/// blendする
		backTex->BlendRenderTexture((*frontTex), outputTex);

		/// output side index を変更する
		outputSideIndex = outputSideIndex == 1 ? 0 : 1;
	}


	/// ---------------------------------------------- /// 
	///		resultにコピーする
	/// ---------------------------------------------- /// 

	RenderTexture* intermediate = intermediates[1 - outputSideIndex];

	/// output : current -> copy に変更
	ONE::DxBarrierCreator::CreateBarrier(
		outputRenderTexture->GetRenderTexResource(),
		outputRenderTexture->currentResourceState,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	outputRenderTexture->currentResourceState = D3D12_RESOURCE_STATE_COPY_DEST;

	/// itnermediate : current -> copy source に変更
	ONE::DxBarrierCreator::CreateBarrier(
		intermediate->GetRenderTexResource(),
		intermediate->currentResourceState,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);
	intermediate->currentResourceState = D3D12_RESOURCE_STATE_COPY_SOURCE;



	/// intermediate -> result へコピーする
	sInstance_.pCommandList_->CopyResource(
		outputRenderTexture->GetRenderTexResource(),
		intermediate->GetRenderTexResource()
	);



	/// output : current -> shader resource に変更
	ONE::DxBarrierCreator::CreateBarrier(
		outputRenderTexture->GetRenderTexResource(),
		outputRenderTexture->currentResourceState,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	outputRenderTexture->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;


	/// itnermediate : current -> shader resource に変更
	ONE::DxBarrierCreator::CreateBarrier(
		intermediate->GetRenderTexResource(),
		intermediate->currentResourceState,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	intermediate->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

}



void RenderTextureManager::BeginRenderTarget(const std::string& name) {
	auto data = sInstance_.renderTexDatas_.at(name);
	auto renderTex = sInstance_.renderTextures_[data.layerNum].get();

	renderTex->BeginRenderTarget();
	data.isRenderTargetActive = true;
}


void RenderTextureManager::EndRenderTarget(const std::string& name) {

	auto data = sInstance_.renderTexDatas_.at(name);
	auto renderTex = sInstance_.renderTextures_[data.layerNum].get();

	renderTex->EndRenderTarget();

	data.isRenderTargetActive = false;

	for(auto& texData : sInstance_.renderTexDatas_) {
		if(texData.first == name) {
			continue;
		}

		/// render targetとしてアクティブであれば
		if(texData.second.isRenderTargetActive) {
			sInstance_.renderTextures_[texData.second.layerNum]->SetRenderTarget();
			return;
		}
	}

}


void RenderTextureManager::CreateRenderTarget(const std::string& name, uint32_t layerNumber, const Vector4& clearColor) {
	std::unique_ptr<RenderTexture> newRenderTex;
	newRenderTex.reset(new RenderTexture());
	newRenderTex->Initialize(clearColor, sInstance_.pCommandList_, sInstance_.pDxDescriptor_);
	ONE::DxBarrierCreator::CreateBarrier(
		newRenderTex->GetRenderTexResource(),
		newRenderTex->currentResourceState,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	newRenderTex->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	sInstance_.renderTextures_.push_back(std::move(newRenderTex));
	sInstance_.renderTexDatas_[name] = {
		.layerNum = layerNumber,
		.isBlending = true
	};
}


void RenderTextureManager::SetIsBlending(const std::string& name, bool isBlending) {
	sInstance_.renderTexDatas_.at(name).isBlending = isBlending;
}

RenderTexture* RenderTextureManager::GetRenderTexture(const std::string& name) {
	auto data = sInstance_.renderTexDatas_.at(name);
	return sInstance_.renderTextures_[data.layerNum].get();
}
