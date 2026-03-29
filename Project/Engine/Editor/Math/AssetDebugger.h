#pragma once

/// std
#include <string>

/// engine
#include "Engine/Asset/Assets/Mateiral/Material.h"
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Editor/Math/AssetPayload.h"

namespace ONEngine {
class AssetCollection;
}

namespace Editor {

/// ////////////////////////////////////////////////////////
/// Assets Debugger用のMath関数群
/// ////////////////////////////////////////////////////////
namespace ImMathf {

void DrawTextureDropSpace(const std::string& _areaName = "DropArea");

/// テクスチャのプレビュー表示
void DrawTexturePreview(const ONEngine::Texture* _texture);

/// テクスチャのドロップ処理
bool HandleTextureDrop(ONEngine::Material* _material);

/// 法線テクスチャのドロップ処理
bool HandleNormalTextureDrop(ONEngine::Material* _material);




/// @brief Materialの編集UIの表示
/// @param _label ヘッダーの名前
/// @param _material 編集対象のMaterialポインタ
/// @param _assetCollection AssetCollectionポインタ
/// @param _isEditNormalTexture ノーマルマップの編集を行うかどうか
/// @return true: 編集が行われた, false: 編集されなかった
bool MaterialEdit(const std::string& _label, ONEngine::Material* _material, ONEngine::AssetCollection* _assetCollection, bool _isEditNormalTexture = true);

}

} /// Editor
