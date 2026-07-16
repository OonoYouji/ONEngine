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


/// ///////////////////////////////////////////////////
/// hlslシェーダーコンパイラー
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class ShaderCompiler
 * @brief DirectX Shader Compiler (DXC) API を用いて、HLSLのコンパイルおよびインクルード処理を制御するコンパイラクラス
 */
class ShaderCompiler final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	ShaderCompiler();

	/**
	 * @brief デストラクタ
	 */
	~ShaderCompiler();

	/**
	 * @brief DXCのユーティリティ（IDxcUtils）、コンパイラ（IDxcCompiler3）、およびデフォルトのインクルードハンドラを初期化します。
	 */
	void Initialize();	

	/**
	 * @brief DirectX Shader Compiler を使用して、HLSLファイルを指定されたプロファイルでコンパイルします。
	 * @param _filePath HLSLファイルへの絶対または相対パス
	 * @param _profile ターゲットとするシェーダプロファイル（例: L"vs_6_5", L"ps_6_5" 等）
	 * @param _entryPoint シェーダのエントリポイント関数名
	 * @return コンパイル成功時はバイナリデータ（IDxcBlob）のComPtr、失敗時は nullptr を返します
	 */
	ComPtr<IDxcBlob> CompileShader(const std::wstring& _filePath, const wchar_t* _profile, const std::wstring& _entryPoint);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<IDxcUtils>          dxcUtils_       = nullptr; ///< DXCユーティリティ
	ComPtr<IDxcCompiler3>      dxcCompiler_    = nullptr; ///< DXCコンパイラコア
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr; ///< インクルードディレクティブ(#include)解決用ハンドラ

};


} /// ONEngine
