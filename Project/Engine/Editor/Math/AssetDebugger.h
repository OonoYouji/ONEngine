﻿#pragma once

/// std
#include <string>

/// engine
#include "Engine/Asset/Assets/Mateiral/Material.h"

/// ----- 前方宣言 ----- ///
namespace ONEngine {

class AssetCollection;

/// ////////////////////////////////////////////////////////
/// Assets Debugger用のMath関数群
/// ////////////////////////////////////////////////////////
namespace ImMathf {

	/// @brief Materialの編集UIの表示
	/// @param _label ヘッダーの名前
	/// @param _material 編集対象のMaterialポインタ
	/// @param _assetCollection AssetCollectionポインタ
	/// @param _isEditNormalTexture ノーマルマップの編集を行うかどうか
	/// @return true: 編集が行われた, false: 編集されなかった
	bool MaterialEdit(const std::string& _label, Material* _material, AssetCollection* _assetCollection, bool _isEditNormalTexture = true);

}

} /// ONEngine
