#pragma once

/// engine
#include "Engine/Asset/Guid/Guid.h"

namespace ONEngine::Asset{

/// ///////////////////////////////////////////////////
/// Assetを共通化するためのインターフェース
/// ///////////////////////////////////////////////////
/**
 * @class IAsset
 * @brief すべてのアセットデータの基底となるインターフェースクラス
 */
class IAsset {
public:
	/// ==================================================
	/// public : methods
	/// ==================================================

	/**
	 * @brief デフォルトコンストラクタ
	 */
	IAsset() = default;

	/**
	 * @brief 仮想デストラクタ
	 */
	virtual ~IAsset() = default;

	/// ==================================================
	/// public : objects
	/// ==================================================

	Guid guid; ///< アセットを一意に識別するためのGUID
};


/// @brief TがIAssetを継承しているかのコンセプト
template <typename T>
concept IsAsset = std::is_base_of_v<IAsset, T>;

} /// ONEngine
