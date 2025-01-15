#pragma once

/// windows
#include <Windows.h>

/// directx
#include <dxcapi.h>

/// std
#include <string>

/// engine
#include "Engine/DirectX12/ComPtr/ComPtr.h"


/// ===================================================
/// hlslシェーダー
/// ===================================================
class Shader final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	enum class Type {
		vs, ps,
	};


public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Shader();
	~Shader();

	void Initialize(class ShaderCompiler* _compiler);

	
	/// <summary>
	/// shaderのコンパイル
	/// </summary>
	/// <param name="_filePath"></param>
	/// <param name="_profile"></param>
	/// <returns></returns>
	bool CompileShader(const std::wstring& _filePath, const wchar_t* _profile, Type _type);


public:

	/// ===================================================
	/// public : objects
	/// ===================================================

	ComPtr<IDxcBlob> vs_ = nullptr;
	ComPtr<IDxcBlob> ps_ = nullptr;

	class ShaderCompiler* pShaderCompiler_ = nullptr;

};