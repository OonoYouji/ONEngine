#include "Line2DRenderer.h"

/// engine
#include "Engine/Graphics/Shader/Shader.h"
#include "Engine/Utility/DebugTools/Assert.h"


Line2DRenderer::Line2DRenderer() {}
Line2DRenderer::~Line2DRenderer() {}

void Line2DRenderer::Initialize(ShaderCompiler* _shaderCompiler, DxDevice* _dxDevice) {

	/// shaderをコンパイル
	Shader shader;
	shader.Initialize(_shaderCompiler);
	shader.CompileShader(L"Assets/Shader/Line2D.vs.hlsl", L"vs_6_0", Shader::Type::vs);
	shader.CompileShader(L"Assets/Shader/Line2D.ps.hlsl", L"ps_6_0", Shader::Type::ps);


	/// pipelineの作成
	pipeline_.reset(new GraphicsPipeline());

	pipeline_->SetShadaer(&shader);

	pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipeline_->AddInputElement("COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
	pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
	pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

	pipeline_->CreatePipeline(_dxDevice);


	/// verticesを最大数分メモリを確保
	vertices_.reserve(kMaxVertexNum_);

	/// vertex bufferの作成
	vertexBuffer_.CreateResource(_dxDevice, sizeof(VertexData) * kMaxVertexNum_);
	vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData_));

	vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
	vbv_.SizeInBytes    = static_cast<UINT>(sizeof(VertexData) * kMaxVertexNum_);
	vbv_.StrideInBytes  = static_cast<UINT>(sizeof(VertexData));

}

void Line2DRenderer::PreDraw([[maybe_unused]] DxCommand* _dxCommand) {

	/// pipelineの設定
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	vertices_.clear(); /// 積んでいたデータをリセット
}

void Line2DRenderer::PostDraw([[maybe_unused]] DxCommand* _dxCommand) {

	/// 頂点データが最大数を超えたらエラーを出す
	Assert(vertices_.size() < kMaxVertexNum_, "Maximum number exceeded");

	/// データがない場合は描画しない
	if (vertices_.empty()) {
		return;
	}

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

void Line2DRenderer::DrawCall() {

	vertices_.push_back(VertexData{ .position = Vec4(0,360,0,1), .color = Vec4(1, 0, 0, 1) });
	vertices_.push_back(VertexData{ .position = Vec4(1280,360,0,1), .color = Vec4(1, 0, 0, 1) });

}