#include "Line2DRenderingPipeline.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Shader/Shader.h"
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/RendererComponents/Primitive/Line2DRenderer.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


Line2DRenderingPipeline::Line2DRenderingPipeline() {}
Line2DRenderingPipeline::~Line2DRenderingPipeline() {}

void Line2DRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{	/// pipelineの作成

		/// shaderをコンパイル
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/Line/Line2D.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"./Packages/Shader/Render/Line/Line2D.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_.reset(new GraphicsPipeline());
		pipeline_->SetShader(&shader);

		/// input element setting
		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		/// rasterizer desc setting
		pipeline_->SetRasterizerDesc({});
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);

		/// topology type setting
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);


		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///< view projection: 0


		/// blend desc setting
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		pipeline_->SetBlendDesc(blendDesc);


		pipeline_->SetRTVNum(1);
		pipeline_->SetRTVFormats({ DXGI_FORMAT_R8G8B8A8_UNORM });

		/// create pipeline
		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}


	/// verticesを最大数分メモリを確保
	vertices_.reserve(kMaxVertexNum_);

	/// vertex bufferの作成
	vertexBuffer_.CreateResource(_dxm->GetDxDevice(), sizeof(VertexData) * kMaxVertexNum_);
	vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData_));

	vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
	vbv_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * kMaxVertexNum_);
	vbv_.StrideInBytes = static_cast<UINT>(sizeof(VertexData));

}

void Line2DRenderingPipeline::Draw(class ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<Line2DRenderer>* line2DRendererArray = _ecs->GetComponentArray<Line2DRenderer>();
	if (!line2DRendererArray || line2DRendererArray->GetUsedComponents().empty()) {
		return;
	}

	/// entityから描画データを取得
	for (auto& lineRenderer : line2DRendererArray->GetUsedComponents()) {
		if (lineRenderer) {
			renderingDataList_.push_back(lineRenderer->GetRenderingDataPtr());
		}
	}

	///< 描画データがない場合は描画しない
	if (renderingDataList_.empty()) {
		return;
	}

	/// rendering data listからデータを取得
	for (RenderingData* renderingData : renderingDataList_) {
		vertices_.insert(vertices_.end(), renderingData->vertices.begin(), renderingData->vertices.end());
	}

	/// 頂点データが最大数を超えたら超過分を消す
	if (vertices_.size() > kMaxVertexNum_) {
		vertices_.resize(kMaxVertexNum_);
	}

	/// 描画データをバッファにコピー
	std::memcpy(mappingData_, vertices_.data(), sizeof(VertexData) * vertices_.size());



	/// draw settings
	auto cmdList = _dxCommand->GetCommandList();

	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	cmdList->IASetVertexBuffers(0, 1, &vbv_);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	/// buffer
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, 0);

	/// 描画
	cmdList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);

	/// 描画データのクリア
	vertices_.clear();
	renderingDataList_.clear();
}

