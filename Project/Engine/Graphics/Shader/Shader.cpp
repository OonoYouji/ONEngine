#include "Shader.h"

using namespace ONEngine;

/// std
#include <vector>

/// engine
#include "ShaderCompiler.h"
#include "Shader.h"

Shader::Shader() = default;
Shader::~Shader() = default;

/**
 * @brief 使用するコンパイラを指定して初期化します。
 */
void Shader::Initialize(ShaderCompiler* _compiler) {
	pShaderCompiler_ = _compiler;
}

/**
 * @brief 指定されたHLSLファイルをコンパイルし、対応するステージにバイナリ（Blob）を保存します。
 */
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
