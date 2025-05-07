#include "MeshShaderTest.h"

void MeshShaderTest::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {
	{

		Shader shader;
		shader.Initialize(_shaderCompiler);

		shader.CompileShader(L"Packages/Shader/MeshShaderTest/MeshShaderTest.ms.hlsl", L"ms.6.0", Shader::Type::ms);

	}
}

void MeshShaderTest::Draw(DxCommand* _dxCommand, EntityComponentSystem* _pEntityComponentSystem, Camera* _camera) {

}
