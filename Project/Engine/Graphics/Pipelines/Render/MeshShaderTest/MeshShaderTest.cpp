#include "MeshShaderTest.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"

void MeshShaderTest::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {
	{

		Shader shader;
		shader.Initialize(_shaderCompiler);

		shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.as.hlsl", L"as_6_5", Shader::Type::as);
		shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.ms.hlsl", L"ms_6_5", Shader::Type::ms);
		shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();

		pipeline_->SetShader(&shader);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}
}

void MeshShaderTest::Draw([[maybe_unused]] DxCommand* _dxCommand, [[maybe_unused]] EntityComponentSystem* _pEntityComponentSystem, [[maybe_unused]] Camera* _camera) {

}
