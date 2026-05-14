#include "EditorUI.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Asset/MaterialManager.h"
#include "Engine/Asset/FontManager.h"
#include "Engine/Asset/AssetDatabase.h"
#include <cstdio>
#include <string>

namespace Engine::Editor {

bool EditorUI::AssetPicker(const char* label, const char* assetType, uint32_t* index) {
    ImGui::PushID(label);
    
    std::string assetName = "None";
    if (*index != 0xFFFFFFFF) {
        std::string typeStr(assetType);
        if (typeStr == "Model") {
            auto model = Asset::AssetManager::GetInstance().GetModelByIndex(*index);
            if (model) assetName = model->GetPath();
        } else if (typeStr == "Material") {
            auto mat = Asset::MaterialManager::GetInstance().GetMaterialByIndex(*index);
            if (mat) assetName = mat->GetPath();
        } else if (typeStr == "Texture") {
            auto tex = Asset::TextureManager::GetInstance().GetTextureByIndex(*index);
            if (tex) assetName = tex->GetPath();
        }
    }

    size_t lastSlash = assetName.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        assetName = assetName.substr(lastSlash + 1);
    }

    char buf[256];
    std::snprintf(buf, sizeof(buf), "%s: %s", assetType, assetName.c_str());
    
    bool changed = false;
    ImGui::Text("%s", label);
    ImGui::SameLine();
    
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    if (ImGui::Button(buf, ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
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

bool EditorUI::AssetPicker(const char* label, const char* assetType, uint64_t* guid) {
    ImGui::PushID(label);
    
    std::string assetPath = Asset::AssetDatabase::GetInstance().GetPathFromGuid(*guid);
    std::string assetName = assetPath.empty() ? "None" : assetPath;

    size_t lastSlash = assetName.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        assetName = assetName.substr(lastSlash + 1);
    }

    char buf[256];
    std::snprintf(buf, sizeof(buf), "%s: %s", assetType, assetName.c_str());
    
    bool changed = false;
    ImGui::Text("%s", label);
    ImGui::SameLine();
    
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    if (ImGui::Button(buf, ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
    }
    ImGui::PopStyleVar();

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ASSET_PATH")) {
            const char* path = (const char*)payload->Data;
            *guid = Asset::AssetDatabase::GetInstance().GetGuidFromPath(path);
            changed = true;
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::PopID();
    return changed;
}

} // namespace Engine::Editor
