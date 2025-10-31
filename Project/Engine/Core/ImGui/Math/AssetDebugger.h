#pragma once

/// std
#include <string>

/// engine
#include "Engine/Asset/Assets/Mateiral/Material.h"

/// ----- 前方宣言 ----- ///
class AssetCollection;

/// ////////////////////////////////////////////////////////
/// Assets Debugger用のMath関数群
/// ////////////////////////////////////////////////////////
namespace ImMathf {

	bool MaterialEdit(const std::string& _label, Material* _material, AssetCollection* _assetCollection, bool _isEditNormalTexture = true);

}