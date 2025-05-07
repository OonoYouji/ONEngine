#include "MeshShaderTest.h"

void MeshShaderTest::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {
	{

		Shader shader;
		shader.Initialize(_shaderCompiler);

		shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.as.hlsl", L"as_6_5", Shader::Type::as);
		shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.ms.hlsl", L"ms_6_5", Shader::Type::ms);
		shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.ps.hlsl", L"ps_6_0", Shader::Type::ps);

	}
}

void MeshShaderTest::Draw(DxCommand* _dxCommand, EntityComponentSystem* _pEntityComponentSystem, Camera* _camera) {

}
