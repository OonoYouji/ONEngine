#include "GridRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"

namespace ONEngine {

GridRenderingPipeline::GridRenderingPipeline() {}
GridRenderingPipeline::~GridRenderingPipeline() {}

void GridRenderingPipeline::Initialize(ShaderCompiler* sc, DxManager* dxm) {

	{
		Shader shader;
		shader.Initialize(sc);
		shader.CompileShader(L"./Packages/Shader/Render/Grid/Grid.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"./Packages/Shader/Render/Grid/Grid.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); /// CBV_VIEW_PROJECTION
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); /// CBV_CAMERA

		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());

		pipeline_->CreatePipeline(dxm->GetDxDevice());
	}

	vertexBuffer.Create(6, dxm->GetDxDevice(), dxm->GetDxCommand());
	vertexBuffer.SetVertices(
		{ { -0.5f, 0.0f,  0.5f, 1.0f }, // 左上
		{    0.5f, 0.0f,  0.5f, 1.0f }, // 右上
		{   -0.5f, 0.0f, -0.5f, 1.0f }, // 左下
		{   -0.5f, 0.0f, -0.5f, 1.0f }, // 左下
		{    0.5f, 0.0f,  0.5f, 1.0f }, // 右上
		{    0.5f, 0.0f, -0.5f, 1.0f } } // 右下
	);
	vertexBuffer.Map();

}

void GridRenderingPipeline::Draw(ECSGroup* ecs, CameraComponent* camera, DxCommand* dxCommand) {

	///
	/// 早期リターンの条件を羅列
	///

	if(!camera) { return; }
	if(!camera->IsMakeViewProjection()) { return; }

	pipeline_->SetPipelineStateForCommandList(dxCommand);

	auto cmdList = dxCommand->GetCommandList();
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, CBV_VIEW_PROJECTION);
	camera->GetCameraPosBuffer().BindForGraphicsCommandList(cmdList, CBV_CAMERA_POSITION);
	vertexBuffer.BindForCommandList(cmdList);

	cmdList->DrawInstanced(6, 1, 0, 0);
}

} /// namespace ONEngine