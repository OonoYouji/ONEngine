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

/// ///////////////////////////////////////////////////
/// アセットの読み込み用クラスのインターフェイス
/// ///////////////////////////////////////////////////
class IAssetLoader {
public:
	virtual ~IAssetLoader() = default;
};


/// ///////////////////////////////////////////////////
/// アセットの読み込み用クラスのテンプレート
/// 各アセットごとに特殊化して使用する
/// ///////////////////////////////////////////////////
template <typename T>
class AssetLoaderT : public IAssetLoader {
public:
	static_assert(IsAsset<T>, "AssetLoader can only be used with Asset types.");

	AssetLoaderT() = default;
	~AssetLoaderT() override = default;

	/// @brief 読み込み用関数
	/// @param _filepath 読み込み対象のファイルパス
	/// @return 読み込んだアセット
	std::optional<T> Load(const std::string& /*_filepath*/) {}

	/// @brief 再読み込み用関数
	/// @param _filepath 再読み込み対象のファイルパス
	/// @return 読み込んだアセット
	std::optional<T> Reload(const std::string& /*_filepath*/, T* /*_src*/) {}
	
};


} /// namespace ONEngine