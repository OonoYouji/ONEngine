#include "Shader.h"

/// engine
#include "ShaderCompiler.h"

Shader::Shader() {}
Shader::~Shader() {}

void Shader::Initialize(ShaderCompiler* _compiler) {
	pShaderCompiler_ = _compiler;
}

bool Shader::CompileShader(const std::wstring& _filePath, const wchar_t* _profile, Type _type) {
	ComPtr<IDxcBlob> shader = pShaderCompiler_->CompileShader(_filePath, _profile);

	switch(_type) {
	case Shader::Type::vs:
		vs_ = shader;
		break;
	case Shader::Type::ps:
		ps_ = shader;
		break;
	}


	return false;
}
