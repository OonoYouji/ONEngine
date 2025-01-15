#pragma once

/// windows
#include <Windows.h>

/// directx
#include <wrl/client.h>
#include <dxcapi.h>

/// engine
#include "Engine/DirectX12/ComPtr/ComPtr.h"


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


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<IDxcUtils>          dxcUtils_       = nullptr;
	ComPtr<IDxcCompiler3>      dxcCompiler_    = nullptr;
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;

};

