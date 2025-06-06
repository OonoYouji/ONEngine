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
		return true;
	case Shader::Type::ps:
		ps_ = shader;
		return true;
	case Shader::Type::cs:
		cs_ = shader;
		return true;
	case Shader::Type::ms:
		ms_ = shader;
		return true;
	case Shader::Type::as:
		as_ = shader;
		return true;
	}


	return false;
}

bool Shader::CompileShader(const std::wstring& _filePath, const wchar_t* _profile, Type _type, const std::wstring& _entryPoint) {
	ComPtr<IDxcBlob> shader = pShaderCompiler_->CompileShader(_filePath, _profile, _entryPoint);

	switch (_type) {
	case Shader::Type::vs:
		vs_ = shader;
		return true;
	case Shader::Type::ps:
		ps_ = shader;
		return true;
	case Shader::Type::cs:
		cs_ = shader;
		return true;
	case Shader::Type::ms:
		ms_ = shader;
		return true;
	case Shader::Type::as:
		as_ = shader;
		return true;
	}

	return false;
}
