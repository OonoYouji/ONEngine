#include "Shader.h"

using namespace ONEngine;

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

	ShaderCompiler::ShaderCompileResult result = pShaderCompiler_->CompileShader(_filePath, _profile, _entryPoint);

	switch (_type) {
	case Shader::Type::vs:
		vs_ = result.shaderBlob;
		vsReflection_ = result.reflectionBlob;
		return true;
	case Shader::Type::ps:
		ps_ = result.shaderBlob;
		psReflection_ = result.reflectionBlob;
		return true;
	case Shader::Type::cs:
		cs_ = result.shaderBlob;
		csReflection_ = result.reflectionBlob;
		return true;
	case Shader::Type::ms:
		ms_ = result.shaderBlob;
		msReflection_ = result.reflectionBlob;
		return true;
	case Shader::Type::as:
		as_ = result.shaderBlob;
		asReflection_ = result.reflectionBlob;
		return true;
	}

	return false;
}
