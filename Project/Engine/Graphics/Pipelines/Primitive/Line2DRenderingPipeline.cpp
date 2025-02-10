#include "Line2DRenderingPipeline.h"

/// engine
#include "Engine/Graphics/Shader/Shader.h"
#include "Engine/Core/Utility/DebugTools/Assert.h"
#include "Engine/Component/RendererComponents/Primitive/Line2DRenderer.h"
#include "Engine/Entity/Collection/EntityCollection.h"


Line2DRenderingPipeline::Line2DRenderingPipeline() {}
Line2DRenderingPipeline::~Line2DRenderingPipeline() {}

void Line2DRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxDevice* _dxDevice) {

	{	/// pipelineの作成
		
		/// shaderをコンパイル
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Assets/Shader/Line2D.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Assets/Shader/Line2D.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_.reset(new GraphicsPipeline());
		pipeline_->SetShader(&shader);

		/// input element setting
		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		/// rasterizer desc setting
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);

		/// topology type setting
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

		/// blend desc setting
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable           = TRUE;
		blendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		pipeline_->SetBlendDesc(blendDesc);


		/// create pipeline
		pipeline_->CreatePipeline(_dxDevice);
	}


	/// verticesを最大数分メモリを確保
	vertices_.reserve(kMaxVertexNum_);

	/// vertex bufferの作成
	vertexBuffer_.CreateResource(_dxDevice, sizeof(VertexData) * kMaxVertexNum_);
	vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData_));

	vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
	vbv_.SizeInBytes    = static_cast<UINT>(sizeof(VertexData) * kMaxVertexNum_);
	vbv_.StrideInBytes  = static_cast<UINT>(sizeof(VertexData));

}


void Line2DRenderingPipeline::Draw(DxCommand* _dxCommand, EntityCollection* _entityCollection) {
	ID3D12GraphicsCommandList* commandList = _dxCommand->GetCommandList();

	/// pre draw
	/// setting
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);



	/// draw
	for (auto& entity : _entityCollection->GetEntities()) {
		Line2DRenderer*&& lineRenderer = entity->GetComponent<Line2DRenderer>();

		if (lineRenderer) {
			renderingDataList_.push_back(lineRenderer->GetRenderingDataPtr());
		}
	}

	if (renderingDataList_.empty()) { ///< 描画データがない場合は描画しない
		return;
	}

	/// rendering data listからデータを取得
	for (RenderingData* renderingData : renderingDataList_) {
		vertices_.insert(vertices_.end(), renderingData->vertices.begin(), renderingData->vertices.end());
	}
	
	if (vertices_.size() > kMaxVertexNum_) { ///< 頂点データが最大数を超えたら超過分を消す
		vertices_.resize(kMaxVertexNum_);
	}

	/// 描画データをバッファにコピー
	std::memcpy(mappingData_, vertices_.data(), sizeof(VertexData) * vertices_.size());

	/// 描画
	commandList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);



	/// post draw
	/// 描画データのクリア
	vertices_.clear();
	renderingDataList_.clear();

}

void Line2DRenderingPipeline::PushBackRenderingData(RenderingData* _renderingData) {
	renderingDataList_.push_back(_renderingData);
}

