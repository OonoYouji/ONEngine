#pragma once

/// std
#include <string>

/// engine
#include "Engine/Asset/Guid/Guid.h"

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// アセットの読み込み用クラスのインターフェイス
/// ///////////////////////////////////////////////////
class IAssetLoader {
public:

	/// @brief 読み込み関数
	/// @param _filepath 対象のファイルパス
	/// @return true: 読み込み成功, false: 読み込み失敗
	virtual bool Load(const std::string& _filepath) = 0;

	/// @brief 再読み込み関数
	/// @param _filepath 対象のファイルパス
	/// @return true: 読み込み成功, false: 読み込み失敗
	virtual bool Reload(const std::string& _filepath) = 0;

	/// @brief 読み込み解除関数
	/// @param _filepath 対象のファイルパス
	/// @return true: 読み込み解除成功, false: 読み込み解除失敗
	virtual bool Unload(const std::string& _filepath) = 0;

};

} /// namespace ONEngine