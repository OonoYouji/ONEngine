#pragma once

/// std
#include <string>

/// engine
#include "Engine/Asset/Assets/Mateiral/Material.h"

namespace ONEngine {
class AssetCollection;
}

namespace Editor {

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
	bool MaterialEdit(const std::string& _label, ONEngine::Material* _material, ONEngine::AssetCollection* _assetCollection, bool _isEditNormalTexture = true);

}

} /// Editor
