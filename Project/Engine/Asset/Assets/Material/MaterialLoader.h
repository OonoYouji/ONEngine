#pragma once

/// engine
#include "../IAssetLoader.h"
#include "../../Meta/MetaFile.h"
#include "Material.h"

namespace ONEngine::Asset {

/**
 * @class AssetLoader<Material>
 * @brief Materialアセット専用のアセットローダーの特殊化クラス
 */
template<>
class AssetLoader<Material> : public IAssetLoader {
public:
	/// ==================================================
	/// public : methods
	/// ==================================================

	/**
	 * @brief デフォルトコンストラクタ
	 */
	AssetLoader() = default;
	
	/**
	 * @brief デストラクタ
	 */
	~AssetLoader() override = default;

	/**
	 * @brief ディスクからマテリアル設定（.mate）ファイルをJSONとしてロードします。
	 * @param _filepath ロード対象のファイルパス
	 * @param meta マテリアルのメタデータ
	 * @return ロードされたMaterialアセット（失敗時はstd::nullopt）
	 */
	[[nodiscard]]
	std::optional<Material> Load(const std::string& _filepath, Meta<typename Material::MetaData> meta);

	/**
	 * @brief 既存のマテリアルに対して再ロード（リロード）を実行します。
	 * @param _filepath 再ロード対象のファイルパス
	 * @param _src 再ロード元のMaterialオブジェクトへのポインタ
	 * @param meta マテリアルのメタデータ
	 * @return 再ロードされたMaterialアセット（失敗時はstd::nullopt）
	 */
	[[nodiscard]]
	std::optional<Material> Reload(const std::string& _filepath, Material* _src = nullptr, Meta<typename Material::MetaData> meta = {});

	/**
	 * @brief マテリアルアセットに対応するメタデータを取得します。
	 * @param _filepath 対象アセットファイルのパス
	 * @return 解析・構築されたメタデータオブジェクト
	 */
	Meta<typename Material::MetaData> GetMetaData(const std::string& _filepath);

};

} /// namespace ONEngine::Asset