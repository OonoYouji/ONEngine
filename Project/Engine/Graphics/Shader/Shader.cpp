#include "Shader.h"

/// std
#include <vector>

/// engine
#include "ShaderCompiler.h"

Shader::Shader() = default;
Shader::~Shader() = default;

void Shader::Initialize(ShaderCompiler* _compiler) {
	pShaderCompiler_ = _compiler;
}

bool Shader::CompileShader(const std::wstring& _filePath, const wchar_t* _profile, Type _type, const std::wstring& _entryPoint) {
	/// ----- Typeごとにコンパイル結果を保存 ----- ///

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
