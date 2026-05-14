#pragma once
#include "imgui.h"
#include <cstdint>

namespace Engine::Editor {

class EditorUI {
public:
    /// @brief アセットを選択するためのUIを表示
    /// @param label ラベル
    /// @param assetType アセットの種類 ("Model", "Texture", etc.)
    /// @param index アセットのインデックス（変更された場合に更新される）
    /// @return 値が変更されたらtrue
    static bool AssetPicker(const char* label, const char* assetType, uint32_t* index);

    /// @brief GUID用アセットピッカー
    static bool AssetPicker(const char* label, const char* assetType, uint64_t* guid);
};

} // namespace Engine::Editor
