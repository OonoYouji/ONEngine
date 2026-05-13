#include "EditorUI.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Asset/MaterialManager.h"
#include <cstdio>
#include <string>

namespace Engine::Editor {

bool EditorUI::AssetPicker(const char* label, const char* assetType, uint32_t* index) {
    ImGui::PushID(label);
    
    std::string assetName = "None";
    if (*index != 0xFFFFFFFF && *index != 0) {
        std::string typeStr(assetType);
        if (typeStr == "Model") {
            auto model = Asset::AssetManager::GetInstance().GetModelByIndex(*index);
            if (model) assetName = model->GetPath();
        } else if (typeStr == "Material") {
            auto mat = Asset::MaterialManager::GetInstance().GetMaterialByIndex(*index);
            if (mat) assetName = mat->GetPath();
        } else if (typeStr == "Texture") {
            // TextureManager doesn't have GetByIndex yet, showing index only for now
            assetName = "Texture " + std::to_string(*index);
        }
    }

    // ファイル名だけを抽出
    size_t lastSlash = assetName.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        assetName = assetName.substr(lastSlash + 1);
    }

    char buf[256];
    std::snprintf(buf, sizeof(buf), "%s: %s", assetType, assetName.c_str());
    
    bool changed = false;
    ImGui::Text("%s", label);
    ImGui::SameLine();
    
    // アセットピッカーのボタン（ドラッグ＆ドロップターゲット）
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    if (ImGui::Button(buf, ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
        // 将来的にはここでアセット選択ブラウザを開く
    }
    ImGui::PopStyleVar();

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ASSET_PATH")) {
            const char* path = (const char*)payload->Data;
            std::string typeStr(assetType);
            
            if (typeStr == "Model") {
                *index = Asset::AssetManager::GetInstance().LoadModel(path);
            } else if (typeStr == "Material") {
                *index = (uint32_t)Asset::MaterialManager::GetInstance().LoadMaterial(path);
            } else if (typeStr == "Texture") {
                *index = (uint32_t)Asset::TextureManager::GetInstance().LoadTexture(path);
            }
            changed = true;
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::PopID();
    return changed;
}

} // namespace Engine::Editor
