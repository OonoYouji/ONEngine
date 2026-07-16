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

/**
 * @class Shader
 * @brief 個々のHLSLシェーダステージ（VS, PS, CS, MS, AS）のコンパイル済みバイナリ（IDxcBlob）を保持・管理するクラス
 */
class Shader final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief シェーダのステージ種類を定義する列挙型
	 */
	enum class Type {
		vs, ///< 頂点シェーダ
		ps, ///< ピクセルシェーダ
		cs, ///< コンピュートシェーダ
		ms, ///< メッシュシェーダ
		as  ///< アンプリフィケーションシェーダ
	};


public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	Shader();

	/**
	 * @brief デストラクタ
	 */
	~Shader();

	/**
	 * @brief 使用するコンパイラを指定して初期化します。
	 * @param _compiler 使用するShaderCompilerインスタンスポインタ
	 */
	void Initialize(class ShaderCompiler* _compiler);


	/**
	 * @brief 指定されたHLSLファイルをコンパイルし、対応するステージにバイナリ（Blob）を保存します。
	 * @param _filePath シェーダファイルへのパス
	 * @param _profile ターゲットプロファイル（例: L"vs_6_5"など）
	 * @param _type コンパイルするシェーダ的ステージ型
	 * @param _entryPoint エントリポイント関数名（デフォルトは L"main"）
	 * @return コンパイル成功すれば true、エラーが発生すれば false
	 */
	bool CompileShader(const std::wstring& _filePath, const wchar_t* _profile, Type _type, const std::wstring& _entryPoint = L"main");


public:

	/// ===================================================
	/// public : objects
	/// ===================================================

	ComPtr<IDxcBlob> vs_; ///< 頂点シェーダバイナリ
	ComPtr<IDxcBlob> ps_; ///< ピクセルシェーダバイナリ
	ComPtr<IDxcBlob> cs_; ///< コンピュートシェーダバイナリ
	ComPtr<IDxcBlob> ms_; ///< メッシュシェーダバイナリ
	ComPtr<IDxcBlob> as_; ///< アンプリフィケーションシェーダバイナリ

	class ShaderCompiler* pShaderCompiler_ = nullptr;


public:

	/// ===================================================
	/// public : accessors
	/// ===================================================

	/**
	 * @brief 頂点シェーダのBlobを取得します。
	 * @return IDxcBlobポインタ
	 */
	IDxcBlob* GetVS() const { return vs_.Get(); }

	/**
	 * @brief ピクセルシェーダのBlobを取得します。
	 * @return IDxcBlobポインタ
	 */
	IDxcBlob* GetPS() const { return ps_.Get(); }

	/**
	 * @brief コンピュートシェーダのBlobを取得します。
	 * @return IDxcBlobポインタ
	 */
	IDxcBlob* GetCS() const { return cs_.Get(); }

	/**
	 * @brief メッシュシェーダのBlobを取得します。
	 * @return IDxcBlobポインタ
	 */
	IDxcBlob* GetMS() const { return ms_.Get(); }

	/**
	 * @brief アンプリフィケーションシェーダのBlobを取得します。
	 * @return IDxcBlobポインタ
	 */
	IDxcBlob* GetAS() const { return as_.Get(); }

};

} /// ONEngine
