#include "SkyboxRenderingPipeline.h"

SkyboxRenderingPipeline::SkyboxRenderingPipeline() {}
SkyboxRenderingPipeline::~SkyboxRenderingPipeline() {}

void SkyboxRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Skybox.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Skybox.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);



	}

}

void SkyboxRenderingPipeline::Draw(DxCommand* _dxCommand, EntityComponentSystem* _pEntityComponentSystem, Camera* _camera) {}
