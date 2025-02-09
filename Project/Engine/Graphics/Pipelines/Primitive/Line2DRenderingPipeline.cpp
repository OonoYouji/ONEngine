#include "Line2DRenderingPipeline.h"

/// engine
#include "Engine/Graphics/Shader/Shader.h"
#include "Engine/Utility/DebugTools/Assert.h"


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

void Line2DRenderingPipeline::PreDraw([[maybe_unused]] DxCommand* _dxCommand) {
	vertices_.clear(); /// 積んでいたデータをリセット
	renderingDataList_.clear(); /// 積んでいたデータをリセット

}

void Line2DRenderingPipeline::PostDraw([[maybe_unused]] DxCommand* _dxCommand) {

	/// 描画データがない場合は描画しない
	if (renderingDataList_.empty()) {
		return;
	}

	/// rendering data listからデータを取得
	for (RenderingData* renderingData : renderingDataList_) {
		vertices_.insert(vertices_.end(), renderingData->vertices.begin(), renderingData->vertices.end());
	}

	/// 頂点データが最大数を超えたらエラーを出す
	Assert(vertices_.size() < kMaxVertexNum_, "Maximum number exceeded");


	/// pipelineの設定
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	

	ID3D12GraphicsCommandList* commandList = _dxCommand->GetCommandList();

	/// データをコピー
	std::memcpy(mappingData_, vertices_.data(), sizeof(VertexData) * vertices_.size());

	/// pipelineのセット
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	/// param setting
	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	/// 描画
	commandList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);

}

void Line2DRenderingPipeline::PushBackRenderingData(RenderingData* _renderingData) {
	renderingDataList_.push_back(_renderingData);
}

