#include "PostProcessLighting.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"

PostProcessLighting::PostProcessLighting() {}
PostProcessLighting::~PostProcessLighting() {}


void PostProcessLighting::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {
	pipeline_ = std::make_unique<ComputePipeline>();

	{	/// shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Assets/Shader/PostProcess/Lighting/Lighting.cs.hlsl", L"cs_6_0", Shader::Type::cs);

		pipeline_->SetShader(&shader);
	
		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}

}

void PostProcessLighting::Execute(DxCommand* _dxCommand) {
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	_dxCommand->GetCommandList()->Dispatch(1280 / 32, 720 / 32, 1);
}