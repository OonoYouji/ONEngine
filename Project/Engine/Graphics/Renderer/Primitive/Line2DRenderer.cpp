#include "Line2DRenderer.h"

#include "Engine/Graphics/Shader/Shader.h"


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

}

void Line2DRenderer::PreDraw([[maybe_unused]] DxCommand* _dxCommand) {

	/// pipelineの設定
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

}

void Line2DRenderer::PostDraw([[maybe_unused]] DxCommand* _dxCommand) {

	/// 今までに積んだデータを実行する



}

void Line2DRenderer::DrawCall() {



}