#pragma once

/// engine
#include "Engine/Asset/Guid/Guid.h"

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// Assetを共通化するためのインターフェース
/// ///////////////////////////////////////////////////
class IAsset {
public:
	/// ==================================================
	/// public : methods
	/// ==================================================

	IAsset() = default;
	virtual ~IAsset() = default;

	/// ==================================================
	/// public : objects
	/// ==================================================

	Guid guid;
};


/// @brief TがIAssetを継承しているかのコンセプト
template <typename T>
concept IsAsset = std::is_base_of_v<IAsset, T>;

} /// ONEngine
