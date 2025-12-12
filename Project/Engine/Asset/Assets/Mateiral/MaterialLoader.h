#pragma once

/// engine
#include "../IAssetLoader.h"
#include "Material.h"

namespace ONEngine {

template<>
class AssetLoaderT<Material> : public IAssetLoader {
public:
	/// ==================================================
	/// public : methods
	/// ==================================================

	AssetLoaderT() = default;
	~AssetLoaderT() override = default;

	/// @brief モデルの読み込みを行う
	/// @param _filepath 対象のファイルパス
	/// @return 読み込んだモデル
	[[nodiscard]]
	std::optional<Material> Load(const std::string& _filepath);

	/// @brief モデルの再読み込みを行う
	/// @param _filepath 対象のファイルパス
	/// @param _src 元のモデル(この関数内で使用されないのでnullptrで良い)
	/// @return 再読み込みしたモデル
	[[nodiscard]]
	std::optional<Material> Reload(const std::string& _filepath, Material* _src = nullptr);

};

} /// namespace ONEngine