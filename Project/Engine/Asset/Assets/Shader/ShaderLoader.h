#pragma once

/// engine
#include "../IAssetLoader.h"
#include "Shader.h"
#include "Engine/Asset/Meta/MetaFile.h"

namespace ONEngine::Asset {


/**
 * @class AssetLoader<Shader>
 * @brief Shaderアセット専用のアセットローダーの特殊化クラス。HLSLファイルをDXC（DirectX Shader Compiler）でコンパイルします。
 */
template<>
class AssetLoader<Shader> : public IAssetLoader {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	AssetLoader();

	/**
	 * @brief デストラクタ
	 */
	~AssetLoader() override = default;


	/**
	 * @brief HLSLファイルをコンパイルし、Shaderオブジェクトとしてロードします。
	 * @param filepath ロード（コンパイル）対象のファイルパス
	 * @param meta シェーダーのメタデータ（エントリポイント、プロファイル等）
	 * @return ロードされたShaderアセット（失敗時はstd::nullopt）
	 */
	std::optional<Shader> Load(const std::string& filepath, typename Meta<Shader::MetaData> meta);

	/**
	 * @brief 既存のShaderに対して再ロード（リビルド・再コンパイル）を実行します。
	 * @param filepath 再ロード対象のファイルパス
	 * @param src 再ロード元のShaderオブジェクトへのポインタ
	 * @param meta シェーダーのメタデータ
	 * @return 再ロードされたShaderアセット（失敗時はstd::nullopt）
	 */
	std::optional<Shader> Reload(const std::string& filepath, Shader* src, typename Meta<Shader::MetaData> meta);

	/**
	 * @brief シェーダーアセットに対応するメタデータを取得します。
	 * @param filepath 対象アセットファイルのパス
	 * @return 解析・構築されたメタデータオブジェクト
	 */
	Meta<typename Shader::MetaData> GetMetaData(const std::string& filepath);


private:

};


} /// namespace ONEngine::Asset