#include "EditorUI.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Asset/MaterialManager.h"
#include "Engine/Asset/FontManager.h"
#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Script/ScriptHost.h"
#include <cstdio>
#include <string>
#include <filesystem>

#include "Engine/Common/Console.h"
#include <format>

#include "EditorContext.h"
#include <set>

namespace Engine::Editor {

bool EditorUI::AssetPicker(const char* label, const char* assetType, uint32_t* index) {
    bool changed = false;
    ImGui::BeginGroup();
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
        } else if (typeStr == "Font") {
            auto font = Asset::FontManager::GetInstance().GetFontByIndex(*index);
            if (font) assetName = font->GetPath();
        }
    }

    size_t lastSlash = assetName.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        assetName = assetName.substr(lastSlash + 1);
    }

    char buf[256];
    std::snprintf(buf, sizeof(buf), "%s: %s", assetType, assetName.c_str());
    
    ImGui::Text("%s", label);
    ImGui::SameLine();
    
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    ImGui::Button(buf, ImVec2(ImGui::GetContentRegionAvail().x, 0));
    ImGui::PopStyleVar();

    if (ImGui::BeginDragDropTarget()) {
        // ドラッグ中のターゲットを強調表示
        ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255), 0.0f, 0, 2.0f);

        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ASSET_PATH");
        if (!payload) payload = ImGui::AcceptDragDropPayload("DND_ASSET_PATHS");

        if (payload) {
            std::string path = (const char*)payload->Data;
            // DND_ASSET_PATHS の場合は最初のパスのみを抽出
            size_t pipePos = path.find('|');
            if (pipePos != std::string::npos) path = path.substr(0, pipePos);

            ::Engine::Console::Log(std::format("AssetPicker [{}]: Dropped path: {}", label, path));

            std::string typeStr(assetType);
            if (typeStr == "Model") {
                *index = Asset::AssetManager::GetInstance().LoadModel(path);
            } else if (typeStr == "Material") {
                *index = (uint32_t)Asset::MaterialManager::GetInstance().LoadMaterial(path);
            } else if (typeStr == "Texture") {
                *index = (uint32_t)Asset::TextureManager::GetInstance().LoadTexture(path);
            } else if (typeStr == "Font") {
                *index = (uint32_t)Asset::FontManager::GetInstance().LoadFont(path);
            }
            changed = true;
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::PopID();
    ImGui::EndGroup();
    return changed;
}

bool EditorUI::AssetPicker(const char* label, const char* assetType, uint64_t* guid) {
    bool changed = false;
    ImGui::BeginGroup();
    ImGui::PushID(label);
    
    std::string assetPath = Asset::AssetDatabase::GetInstance().GetPathFromGuid(*guid);
    std::string assetName = assetPath.empty() ? "None" : assetPath;

    size_t lastSlash = assetName.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        assetName = assetName.substr(lastSlash + 1);
    }

    char buf[256];
    std::snprintf(buf, sizeof(buf), "%s: %s", assetType, assetName.c_str());
    
    ImGui::Text("%s", label);
    ImGui::SameLine();
    
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    ImGui::Button(buf, ImVec2(ImGui::GetContentRegionAvail().x, 0));
    ImGui::PopStyleVar();

    if (ImGui::BeginDragDropTarget()) {
        // ドラッグ中のターゲットを強調表示
        ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255), 0.0f, 0, 2.0f);

        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ASSET_PATH");
        if (!payload) payload = ImGui::AcceptDragDropPayload("DND_ASSET_PATHS");

        if (payload) {
            std::string path = (const char*)payload->Data;
            // DND_ASSET_PATHS の場合は最初のパスのみを抽出
            size_t pipePos = path.find('|');
            if (pipePos != std::string::npos) path = path.substr(0, pipePos);

            ::Engine::Console::Log(std::format("AssetPicker [{}]: Dropped path: {}", label, path));

            *guid = Asset::AssetDatabase::GetInstance().GetGuidFromPath(path);
            changed = true;
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::PopID();
    ImGui::EndGroup();
    return changed;
}

bool EditorUI::ScriptPicker(const char* label, uint32_t entityId) {
    bool changed = false;
    ImGui::BeginGroup();
    ImGui::PushID(label);

    std::string scriptName = "None";
    
    // 現在アタッチされているスクリプト名の取得を試みる
    auto& host = Engine::Script::ScriptHost::GetInstance();
    auto getScriptNameFunc = (void(*)(uint32_t, char*, uint32_t))host.GetMethodDelegate(
        L"ONEngine.Scripting.EngineHost, ONEngine.Scripting",
        L"GetScriptNameByEntity",
        L"");

    if (getScriptNameFunc) {
        char buffer[256] = { 0 };
        getScriptNameFunc(entityId, buffer, sizeof(buffer));
        if (buffer[0] != '\0') scriptName = buffer;
    }

    ImGui::Text("%s", label);
    ImGui::SameLine();

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    std::string btnText = scriptName == "None" ? "Drop .cs Script Here" : scriptName;
    ImGui::Button(btnText.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0));
    ImGui::PopStyleVar();

    if (ImGui::BeginDragDropTarget()) {
        ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255), 0.0f, 0, 2.0f);

        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ASSET_PATH");
        if (!payload) payload = ImGui::AcceptDragDropPayload("DND_ASSET_PATHS");

        if (payload) {
            std::string pathStr = (const char*)payload->Data;
            size_t pipePos = pathStr.find('|');
            if (pipePos != std::string::npos) pathStr = pathStr.substr(0, pipePos);

            std::filesystem::path path(pathStr);
            if (path.extension() == ".cs") {
                std::string scriptName = path.stem().string();
                ::Engine::Console::Log(std::format("ScriptPicker: Detected .cs file drop. ScriptName: {}", scriptName));
                
                auto& host = Engine::Script::ScriptHost::GetInstance();
                auto addScriptFunc = (void(*)(uint32_t, const char*, const char*))host.GetMethodDelegate(
                    L"ONEngine.Scripting.EngineHost, ONEngine.Scripting",
                    L"AddScriptByName",
                    L"");

                if (addScriptFunc) {
                    ::Engine::Console::Log("ScriptPicker: Successfully retrieved AddScriptByName delegate.");
                    auto& selection = EditorContext::GetInstance().GetSelection();
                    if (selection.empty()) {
                        ::Engine::Console::Log(std::format("ScriptPicker: No selection, attaching to entityId {}", entityId));
                        addScriptFunc(entityId, scriptName.c_str(), "{}");
                    } else {
                        ::Engine::Console::Log(std::format("ScriptPicker: Attaching to {} selected entities.", selection.size()));
                        for (auto e : selection) {
                            addScriptFunc((uint32_t)e, scriptName.c_str(), "{}");
                        }
                    }
                    ::Engine::Console::Log(std::format("ScriptPicker: Finished calling C# AddScriptByName for '{}'.", scriptName));
                    changed = true;
                } else {
                    ::Engine::Console::LogError("ScriptPicker: FAILED to retrieve AddScriptByName delegate from C#.");
                }
            } else {
                ::Engine::Console::LogWarning(std::format("ScriptPicker: Dropped file is not a .cs script: {}", path.string()));
            }
        }
        ImGui::EndDragDropTarget();
    }

    // [SerializeField] フィールドの表示
    if (scriptName != "None") {
        auto getFieldsFunc = (void(*)(uint32_t, char*, uint32_t))host.GetMethodDelegate(
            L"ONEngine.Scripting.EngineHost, ONEngine.Scripting",
            L"GetScriptFields",
            L"");

        if (getFieldsFunc) {
            char jsonBuffer[4096] = { 0 };
            getFieldsFunc(entityId, jsonBuffer, sizeof(jsonBuffer));

            if (jsonBuffer[0] != '\0') {
                try {
                    auto fields = nlohmann::json::parse(jsonBuffer);
                    if (fields.is_array()) {
                        ImGui::Indent();
                        for (auto& f : fields) {
                            std::string f_name = f["name"];
                            std::string f_type = f["type"];
                            auto f_value = f["value"];

                            bool fieldChanged = false;
                            std::string valueJson;

                            if (f_type == "Single") {
                                float val = f_value.get<float>();
                                if (ImGui::DragFloat(f_name.c_str(), &val, 0.1f)) {
                                    fieldChanged = true;
                                    valueJson = std::to_string(val);
                                }
                            } else if (f_type == "Int32") {
                                int val = f_value.get<int>();
                                if (ImGui::InputInt(f_name.c_str(), &val)) {
                                    fieldChanged = true;
                                    valueJson = std::to_string(val);
                                }
                            } else if (f_type == "Boolean") {
                                bool val = f_value.get<bool>();
                                if (ImGui::Checkbox(f_name.c_str(), &val)) {
                                    fieldChanged = true;
                                    valueJson = val ? "true" : "false";
                                }
                            } else if (f_type == "String") {
                                std::string val = f_value.get<std::string>();
                                char buf[256];
                                strcpy_s(buf, val.c_str());
                                if (ImGui::InputText(f_name.c_str(), buf, sizeof(buf))) {
                                    fieldChanged = true;
                                    valueJson = "\"" + std::string(buf) + "\"";
                                }
                            }

                            if (fieldChanged) {
                                auto setFieldFunc = (void(*)(uint32_t, const char*, const char*))host.GetMethodDelegate(
                                    L"ONEngine.Scripting.EngineHost, ONEngine.Scripting",
                                    L"SetScriptField",
                                    L"");
                                if (setFieldFunc) {
                                    auto& selection = EditorContext::GetInstance().GetSelection();
                                    if (selection.empty()) {
                                        setFieldFunc(entityId, f_name.c_str(), valueJson.c_str());
                                    } else {
                                        for (auto e : selection) {
                                            setFieldFunc((uint32_t)e, f_name.c_str(), valueJson.c_str());
                                        }
                                    }
                                    changed = true;
                                }
                            }
                        }
                        ImGui::Unindent();
                    }
                } catch (...) {}
            }
        }
    }

    ImGui::PopID();
    ImGui::EndGroup();
    return changed;
}

} // namespace Engine::Editor
