#pragma once

/// windows
#include <Windows.h>

/// directx
#include <dxcapi.h>

/// std
#include <string>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"


/// ===================================================
/// hlslシェーダー
/// ===================================================
class Shader final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	enum class Type {
		vs, ps, cs, ms, as
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
	ComPtr<IDxcBlob> cs_ = nullptr;
	ComPtr<IDxcBlob> ms_ = nullptr;
	ComPtr<IDxcBlob> as_ = nullptr;

	class ShaderCompiler* pShaderCompiler_ = nullptr;


public:

	/// ===================================================
	/// public : accessors
	/// ===================================================

	IDxcBlob* GetVS() const { return vs_.Get(); }
	IDxcBlob* GetPS() const { return ps_.Get(); }
	IDxcBlob* GetCS() const { return cs_.Get(); }

};
