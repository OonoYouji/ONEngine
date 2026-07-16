#pragma once

/// std
#include <string>
#include <optional>

/// engine
#include "Engine/Asset/Guid/Guid.h"
#include "IAsset.h"

namespace ONEngine {
class DxManager;
class AssetCollection;
}

namespace ONEngine::Asset {
template <typename T>
struct Meta;
}

namespace ONEngine::Asset {

/**
 * @class IAssetLoader
 * @brief すべてのアセットローダーの共通インターフェース基底クラス
 */
class IAssetLoader {
public:
	/**
	 * @brief 仮想デストラクタ
	 */
	virtual ~IAssetLoader() = default;
};


/**
 * @class AssetLoader
 * @brief 各アセットタイプに対応するローダーのテンプレートクラス。各アセット毎に特殊化して定義します。
 * @tparam T IAssetを継承したアセットタイプ
 */
template <typename T>
class AssetLoader : public IAssetLoader {
public:
	static_assert(IsAsset<T>, "AssetLoader can only be used with Asset types.");

	/**
	 * @brief デフォルトコンストラクタ
	 */
	AssetLoader() = default;

	/**
	 * @brief デストラクタ
	 */
	~AssetLoader() override = default;

	/**
	 * @brief ディスクからアセットをロードします。
	 * @param _filepath ロード対象のファイルパス
	 * @param meta アセットに対応するメタデータ
	 * @return ロードに成功した場合はアセットのオプショナルオブジェクト、失敗した場合はstd::nullopt
	 */
	std::optional<T> Load(const std::string& /*_filepath*/, Meta<typename T::MetaData> /*meta*/) {}

	/**
	 * @brief 既存のアセットに対してデータを再ロード（リロード）します。
	 * @param _filepath 再ロード対象のファイルパス
	 * @param _src 再ロード元となるアセットオブジェクトのポインタ
	 * @param meta アセットに対応するメタデータ
	 * @return 再ロードに成功した場合はアセットのオプショナルオブジェクト、失敗した場合はstd::nullopt
	 */
	std::optional<T> Reload(const std::string& /*_filepath*/, T* /*_src*/, Meta<typename T::MetaData> /*meta*/) {}

	/**
	 * @brief アセットのメタデータを解析または生成して取得します。
	 * @param _filepath 対象アセットファイルのパス
	 * @return 解析・構築されたメタデータオブジェクト
	 */
	Meta<typename T::MetaData> GetMetaData(const std::string& /*_filepath*/) { return {}; }

};


} /// namespace ONEngine