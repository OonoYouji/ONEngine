#pragma once

/// windows
#include <Windows.h>

/// directx
#include <wrl/client.h>
#include <dxcapi.h>

/// std
#include <string>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"


/// ===================================================
/// hlslシェーダーコンパイラー
/// ===================================================
class ShaderCompiler final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ShaderCompiler();
	~ShaderCompiler();

	/// <summary>
	/// このクラスの初期化関数
	/// </summary>
	void Initialize();	

	/// <summary>
	/// shaderのコンパイル
	/// </summary>
	/// <param name="_filePath"></param>
	/// <param name="_profile"></param>
	/// <returns></returns>
	ComPtr<IDxcBlob> CompileShader(const std::wstring& _filePath, const wchar_t* _profile);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<IDxcUtils>          dxcUtils_       = nullptr;
	ComPtr<IDxcCompiler3>      dxcCompiler_    = nullptr;
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;

};

