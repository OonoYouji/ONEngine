#pragma once

/// std
#include <string>

/// engine
#include "Engine/Asset/Assets/Material/Material.h"
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Editor/Math/AssetPayload.h"

namespace ONEngine::Asset {
class AssetCollection;
}

namespace Editor {

/// ////////////////////////////////////////////////////////
/// Assets Debugger用のMath関数群
/// ////////////////////////////////////////////////////////
namespace ImMathf {

void DrawTextureDropSpace(const std::string& areaName = "DropArea");

/// テクスチャのプレビュー表示
void DrawTexturePreview(const ONEngine::Asset::Texture* texture);

/// @brief テクスチャボタンの表示
/// @param texture ボタンとして描画したいテクスチャのポインタ
/// @return true: ボタンを押した false: ボタンを押していない
bool TextureButton(const std::string& label, const ONEngine::Asset::Texture* texture);

/// テクスチャのドロップ処理
bool HandleTextureDrop(ONEngine::Asset::Material* material);

/// 法線テクスチャのドロップ処理
bool HandleNormalTextureDrop(ONEngine::Asset::Material* material);




/// @brief Materialの編集UIの表示
/// @param _label ヘッダーの名前
/// @param _material 編集対象のMaterialポインタ
/// @param _assetCollection AssetCollectionポインタ
/// @param _isEditNormalTexture ノーマルマップの編集を行うかどうか
/// @return true: 編集が行われた, false: 編集されなかった
bool MaterialEdit(const std::string& label, ONEngine::Asset::Material* material, ONEngine::Asset::AssetCollection* assetCollection, bool isEditNormalTexture = true);

}

} /// Editor
