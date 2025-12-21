#pragma once

/// editor
#include "Engine/Editor/Math/AssetPayload.h"

namespace ONEngine {
/// 前方宣言
class AssetCollection;
} /// namespace ONEngine


namespace Editor {
namespace DragDrop {


/// @brief ドラッグ用のペイロードにデータを設定する
/// @param _filepath 持っているファイルパス
/// @param _guid 持っているアセットのGuid
void SetDragDropPayload(const std::string& _filepath, const ONEngine::Guid& _guid, ONEngine::AssetCollection* _ac);


/// @brief ドラッグしているペイロードを取得する
/// @return ドラッグ中のペイロードへのポインタ (存在しない場合はnullptr)
AssetPayload* GetDragDropPayload();


} /// namespace DragDrop
} /// namespace Editor


