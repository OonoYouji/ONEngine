#pragma once

/// windows
#include <Windows.h>

/// directx
#include <dxcapi.h>

/// std
#include <string>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"


/// ///////////////////////////////////////////////////
/// hlslシェーダー
/// ///////////////////////////////////////////////////
namespace ONEngine {

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
	bool CompileShader(const std::wstring& _filePath, const wchar_t* _profile, Type _type, const std::wstring& _entryPoint = L"main");


public:

	/// ===================================================
	/// public : objects
	/// ===================================================

	ComPtr<IDxcBlob> vs_;
	ComPtr<IDxcBlob> ps_;
	ComPtr<IDxcBlob> cs_;
	ComPtr<IDxcBlob> ms_;
	ComPtr<IDxcBlob> as_;

	ComPtr<IDxcBlob> vsReflection_;
	ComPtr<IDxcBlob> psReflection_;
	ComPtr<IDxcBlob> csReflection_;
	ComPtr<IDxcBlob> msReflection_;
	ComPtr<IDxcBlob> asReflection_;

	class ShaderCompiler* pShaderCompiler_ = nullptr;


public:

	/// ===================================================
	/// public : accessors
	/// ===================================================

	IDxcBlob* GetVS() const { return vs_.Get(); }
	IDxcBlob* GetPS() const { return ps_.Get(); }
	IDxcBlob* GetCS() const { return cs_.Get(); }
	IDxcBlob* GetMS() const { return ms_.Get(); }
	IDxcBlob* GetAS() const { return as_.Get(); }

	IDxcBlob* GetVSReflection() const { return vsReflection_.Get(); }
	IDxcBlob* GetPSReflection() const { return psReflection_.Get(); }
	IDxcBlob* GetCSReflection() const { return csReflection_.Get(); }
	IDxcBlob* GetMSReflection() const { return msReflection_.Get(); }
	IDxcBlob* GetASReflection() const { return asReflection_.Get(); }

};

} /// ONEngine
