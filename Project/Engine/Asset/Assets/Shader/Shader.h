#pragma once

/// dx
#include <Windows.h>
#include <dxcapi.h>

/// std
#include <string>

/// engine
#include "../IAsset.h"
#include "../../../Core/DirectX12/ComPtr/ComPtr.h"



namespace ONEngine::Asset {
template<typename T>
class AssetLoader;
}



namespace ONEngine::Asset {



/**
 * @enum ShaderStage
 * @brief シェーダーのパイプラインステージ（種類）を表す列挙型
 */
enum class ShaderStage {
	Unkown,        ///< 未定義のステージ
	Vertex,        ///< 頂点シェーダー
	Pixel,         ///< ピクセル（フラグメント）シェーダー
	Compute,       ///< コンピュートシェーダー
	Amplification, ///< アンプリフィケーションシェーダー（メッシュパイプライン用）
	Mesh,          ///< メッシュシェーダー（メッシュパイプライン用）
};

/**
 * @brief jsonオブジェクトからShaderStage列挙型へのデシリアライズを行います。
 * @param j jsonオブジェクト
 * @param stage 変換先ShaderStageの参照
 */
void from_json(const nlohmann::json& j, ShaderStage& stage);

/**
 * @brief ShaderStage列挙型からjsonオブジェクトへのシリアライズを行います。
 * @param j jsonオブジェクト
 * @param stage 変換元ShaderStage
 */
void to_json(nlohmann::json& j, const ShaderStage& stage);


/**
 * @class Shader
 * @brief ビルド（コンパイル）されたシェーダーオブジェクトを管理するアセットクラス
 */
class Shader : public IAsset {
	friend class AssetLoader<Shader>;
public:
	/// ===================================================
	/// public : sub class
	/// ===================================================

	/**
	 * @struct MetaData
	 * @brief シェーダーアセット固有のメタデータ（ステージ、エントリポイントなど）
	 */
	struct MetaData {
		ShaderStage stage;       ///< シェーダーの種類
		std::string entryPoint;  ///< エントリポイント関数名（"main" 等）
		std::string profile;     ///< コンパイルターゲット（"vs_6_0", "ps_6_0" 等）
	};


public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief デフォルトコンストラクタ
	 */
	Shader();

	/**
	 * @brief デストラクタ
	 */
	~Shader();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string path_;
	ShaderStage stage_;
	std::string entryPoint_;
	std::string profile_;

	ComPtr<IDxcBlob> shader_;
};

} /// namespace ONEngine