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

	auto frontItr = values.begin();
	auto nextItr = std::next(frontItr);

	auto tex = intermediateTextures_[intermediateIndex_].get();
	ONE::DxBarrierCreator::CreateBarrier(
		tex->GetRenderTexResource(),
		tex->currentResourceState,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	tex->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;


	intermediateIndex_ = 0;
	renderTextures_[(*frontItr)]->BlendRenderTexture(renderTextures_[(*nextItr)].get(), intermediateTextures_[intermediateIndex_].get());

	ONE::DxBarrierCreator::CreateBarrier(
		tex->GetRenderTexResource(),
		tex->currentResourceState,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	tex->currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;


	for(auto itr = nextItr; itr != values.end(); ++itr) {
		auto next = std::next(itr);
		if(next == values.end()) { break; }

		if(intermediateIndex_ == 1) {
			intermediateIndex_ = 0;
		} else {
			intermediateIndex_ = 1;
		}

		auto back = intermediateTextures_[1 - intermediateIndex_].get();
		auto output = intermediateTextures_[intermediateIndex_].get();

		ONE::DxBarrierCreator::CreateBarrier(
			back->GetRenderTexResource(),
			back->currentResourceState,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		back->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		ONE::DxBarrierCreator::CreateBarrier(
			output->GetRenderTexResource(),
			output->currentResourceState,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		output->currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;


		back->BlendRenderTexture(renderTextures_[(*next)].get(), output);

		ONE::DxBarrierCreator::CreateBarrier(
			back->GetRenderTexResource(),
			back->currentResourceState,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		back->currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

		ONE::DxBarrierCreator::CreateBarrier(
			output->GetRenderTexResource(),
			output->currentResourceState,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		output->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;



	}


	//// 中間リソースを最終リソースにコピー

	auto intermediate = intermediateTextures_[intermediateIndex_].get();
	auto finalResource = finalRenderTex_.get();

	ONE::DxBarrierCreator::CreateBarrier(
		intermediate->GetRenderTexResource(),
		intermediate->currentResourceState,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);
	intermediate->currentResourceState = D3D12_RESOURCE_STATE_COPY_SOURCE;

	ONE::DxBarrierCreator::CreateBarrier(
		finalResource->GetRenderTexResource(),
		finalResource->currentResourceState,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	finalResource->currentResourceState = D3D12_RESOURCE_STATE_COPY_DEST;


	/// resource copy
	pCommandList_->CopyResource(finalResource->GetRenderTexResource(), intermediate->GetRenderTexResource());

	ONE::DxBarrierCreator::CreateBarrier(
		intermediate->GetRenderTexResource(),
		intermediate->currentResourceState,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	intermediate->currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

	ONE::DxBarrierCreator::CreateBarrier(
		finalResource->GetRenderTexResource(),
		finalResource->currentResourceState,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	finalResource->currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;


	ONE::DxBarrierCreator::CreateBarrier(
		intermediateTextures_[1 - intermediateIndex_]->GetRenderTexResource(),
		intermediateTextures_[1 - intermediateIndex_]->currentResourceState,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	intermediateTextures_[1 - intermediateIndex_]->currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;


}

void RenderTextureManager::BindForCommandList() {
	pipeline_->SetPipelineState();
	pCommandList_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList_->IASetVertexBuffers(0, 1, &vbv_);

	/// view projection
	pCommandList_->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_->GetGPUVirtualAddress());
}



RenderTexture* RenderTextureManager::CreateBlendRenderTexture(std::vector<RenderTexture*> renderTextures) {

	if(renderTextures.size() < 2) {
		assert(false);
	}

	/// return する render target の初期化
	RenderTexture* result = new RenderTexture;
	result->Initialize(Vec4(0, 0, 0, 0), sInstance_.pCommandList_, sInstance_.pDxDescriptor_);

	/// output に render するための中間リソース
	uint8_t frontTexIndex = 0;		/// frontのtexIndex
	uint8_t outputSideIndex = 0;	/// output先のtexIndex
	std::array<std::unique_ptr<RenderTexture>, 2> intermediates;
	for(auto& intermediate : intermediates) {
		intermediate.reset(new RenderTexture);
		intermediate->Initialize(Vec4(0, 0, 0, 0), sInstance_.pCommandList_, sInstance_.pDxDescriptor_);
	}


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

	renderTextures[1 - frontTexIndex]->BlendRenderTexture(
		renderTextures[frontTexIndex], intermediates[outputSideIndex].get());

	ONE::DxBarrierCreator::CreateBarrier(
		intermediates[outputSideIndex]->GetRenderTexResource(),
		intermediates[outputSideIndex]->currentResourceState,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	intermediates[outputSideIndex]->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;


	for(auto frontTex = (renderTextures.begin() + 2); frontTex != renderTextures.end(); ++frontTex) {

		/// back と outputへのポインタ
		RenderTexture* backTex = intermediates[1 - outputSideIndex].get();
		RenderTexture* outputTex = intermediates[outputSideIndex].get();

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

	RenderTexture* intermediate = intermediates[frontTexIndex].get();

	/// result : current -> copy に変更
	ONE::DxBarrierCreator::CreateBarrier(
		result->GetRenderTexResource(),
		result->currentResourceState,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	result->currentResourceState = D3D12_RESOURCE_STATE_COPY_DEST;

	/// itnermediate : current -> copy source に変更
	ONE::DxBarrierCreator::CreateBarrier(
		intermediate->GetRenderTexResource(),
		intermediate->currentResourceState,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);
	intermediate->currentResourceState = D3D12_RESOURCE_STATE_COPY_SOURCE;



	/// intermediate -> result へコピーする
	sInstance_.pCommandList_->CopyResource(
		result->GetRenderTexResource(),
		intermediate->GetRenderTexResource()
	);



	/// result : current -> shader resource に変更
	ONE::DxBarrierCreator::CreateBarrier(
		result->GetRenderTexResource(),
		result->currentResourceState,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	result->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;


	/// itnermediate : current -> shader resource に変更
	ONE::DxBarrierCreator::CreateBarrier(
		intermediate->GetRenderTexResource(),
		intermediate->currentResourceState,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	intermediate->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;



	return result;
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

RenderTexture* RenderTextureManager::GetRenderTarget(const std::string& name) {
	auto data = sInstance_.renderTexDatas_.at(name);
	return sInstance_.renderTextures_[data.layerNum].get();
}
