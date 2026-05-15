#include "InspectorView.h"
#include "imgui.h"
#include "EditorContext.h"
#include "EditorUtils.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"
#include "ChangeComponentCommand.h"
#include "AddComponentCommand.h"
#include "RemoveComponentCommand.h"
#include "Engine/Core/Application.h"
#include <vector>
#include <string>

namespace Engine::Editor {

using json = nlohmann::json;
static json s_oldState;

void InspectorView::Render(ECS::Registry& registry) {
    ImGui::Begin("Inspector");

    auto entity = EditorContext::GetInstance().GetSelectedEntity();
    if (entity == 0) {
        ImGui::Text("No Entity Selected");
        EditorUtils::DrawActiveViewOutline();
        ImGui::End();
        return;
    }

    auto& compReg = ECS::ComponentRegistry::GetInstance();
    auto& history = CommandHistory::GetInstance();

    // Undo/Redo ボタン
    ImGui::BeginDisabled(!history.CanUndo());
    if (ImGui::Button("Undo")) history.Undo();
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(!history.CanRedo());
    if (ImGui::Button("Redo")) history.Redo();
    ImGui::EndDisabled();
    
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z)) history.Undo();
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y)) history.Redo();

    ImGui::Separator();

    // Entity ID表示と名前編集
    ImGui::Text("Entity ID: %u", entity);
    
    // Tagコンポーネントがない場合は強制的に追加（isActive/Name管理のため）
    if (!registry.HasComponent<ECS::Tag>(entity)) {
        auto& tag = registry.AddComponent<ECS::Tag>(entity);
        tag.isActive = 1;
        sprintf_s(tag.name, "Entity %u", entity);
    }

    auto& tag = registry.GetComponent<ECS::Tag>(entity);

    // --- Entity Active Toggle ---
    bool active = tag.isActive != 0;
    if (ImGui::Checkbox("##EntityActive", &active)) {
        s_oldState = compReg.SerializeComponent(registry, entity, 100);
        tag.isActive = active ? 1 : 0;
        json newState = compReg.SerializeComponent(registry, entity, 100);
        history.Execute(std::make_shared<ChangeComponentCommand>(entity, 100, s_oldState, newState));
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Entity Active/Inactive");

    ImGui::SameLine();
    
    // --- Entity Name ---
    char buffer[256];
    strcpy_s(buffer, tag.name);
    
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##EntityName", buffer, sizeof(buffer), ImGuiInputTextFlags_AutoSelectAll)) {
        if (strcmp(buffer, tag.name) != 0) {
            tag.name[0] = '\0'; // Trigger change for undo if we were to use snapshots, but here we use DeactivatedAfterEdit
            strcpy_s(tag.name, buffer);
        }
    }
    if (ImGui::IsItemActivated()) {
        s_oldState = compReg.SerializeComponent(registry, entity, 100);
    }
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        json newState = compReg.SerializeComponent(registry, entity, 100);
        history.Execute(std::make_shared<ChangeComponentCommand>(entity, 100, s_oldState, newState));
    }
    ImGui::PopItemWidth();
    
    ImGui::Separator();

    auto DrawComponent = [&](uint32_t typeId, const char* name, auto drawUI) {
        auto* info = compReg.GetInfo(typeId);
        if (!info) return;

        auto& storage = info->getStorageFunc(registry);
        if (!storage.Has(entity)) return;

        bool open = true;
        if (ImGui::CollapsingHeader(name, &open, ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID(typeId);
            void* comp = storage.GetRaw(entity);
            
            auto Property = [&](const char* label, auto widgetFunc) {
                json potentialOldState = compReg.SerializeComponent(registry, entity, typeId);
                
                ImGui::PushID(label);
                bool changed = widgetFunc();

                if (ImGui::IsItemActivated()) {
                    s_oldState = potentialOldState;
                }

                // Edit終了時、またはドラッグ＆ドロップなどの即時変更時にコマンドを発行
                if (ImGui::IsItemDeactivatedAfterEdit() || (changed && !ImGui::IsItemActive())) {
                    // ドラッグ＆ドロップ等の単発変更の場合、s_oldStateが以前の別の操作のままになっている可能性があるため、
                    // potentialOldState（変更直前の状態）を優先して使用する。
                    json oldState = s_oldState;
                    if (changed && !ImGui::IsItemActive()) {
                        oldState = potentialOldState;
                    }

                    json newState = compReg.SerializeComponent(registry, entity, typeId);
                    history.Execute(std::make_shared<ChangeComponentCommand>(entity, typeId, oldState, newState));
                    
                    // 次の操作のためにリセット（任意だが安全のため）
                    s_oldState = newState;
                }
                ImGui::PopID();
                return changed;
            };

            drawUI(comp, Property);

            ImGui::PopID();
        }

        if (!open) {
            // Tag(100) と Transform(1) は基本削除不可にする
            if (typeId != 100 && typeId != 1) {
                history.Execute(std::make_shared<RemoveComponentCommand>(entity, typeId));
            }
        }
    };

    // ID 100: Tag (Inspector上部の名前編集と重複するため、ここでは非表示にするか、詳細として残す)
    // ID 1: Transform
    DrawComponent(1, "Transform", [](void* data, auto Prop) {
        ECS::DrawUI_Transform(*static_cast<ECS::Transform*>(data), Prop);
    });

    // ID 2: MeshRenderer
    DrawComponent(2, "MeshRenderer", [](void* data, auto Prop) {
        ECS::DrawUI_MeshRenderer(*static_cast<ECS::MeshRenderer*>(data), Prop);
    });

    // ID 3: ScriptComponent
    DrawComponent(3, "ScriptComponent", [](void* data, auto Prop) {
        ECS::DrawUI_ScriptComponent(*static_cast<ECS::ScriptComponent*>(data), Prop);
    });

    // ID 4: Camera
    DrawComponent(4, "Camera", [](void* data, auto Prop) {
        ECS::DrawUI_Camera(*static_cast<ECS::Camera*>(data), Prop);
    });

    // ID 5: DirectionalLight
    DrawComponent(5, "DirectionalLight", [](void* data, auto Prop) {
        ECS::DrawUI_DirectionalLight(*static_cast<ECS::DirectionalLight*>(data), Prop);
    });

    // ID 6: PointLight
    DrawComponent(6, "PointLight", [](void* data, auto Prop) {
        ECS::DrawUI_PointLight(*static_cast<ECS::PointLight*>(data), Prop);
    });

    // ID 7: SpriteRenderer
    DrawComponent(7, "SpriteRenderer", [](void* data, auto Prop) {
        ECS::DrawUI_SpriteRenderer(*static_cast<ECS::SpriteRenderer*>(data), Prop);
    });

    // ID 8: ParticleEmitter
    DrawComponent(8, "ParticleEmitter", [](void* data, auto Prop) {
        ECS::DrawUI_ParticleEmitter(*static_cast<ECS::ParticleEmitter*>(data), Prop);
    });

    // ID 9: Skybox
    DrawComponent(9, "Skybox", [](void* data, auto Prop) {
        ECS::DrawUI_Skybox(*static_cast<ECS::Skybox*>(data), Prop);
    });

    // ID 10: TextRenderer
    DrawComponent(10, "TextRenderer", [](void* data, auto Prop) {
        ECS::DrawUI_TextRenderer(*static_cast<ECS::TextRenderer*>(data), Prop);
    });

    // ID 11: SkinnedMeshRenderer
    DrawComponent(11, "SkinnedMeshRenderer", [](void* data, auto Prop) {
        ECS::DrawUI_SkinnedMeshRenderer(*static_cast<ECS::SkinnedMeshRenderer*>(data), Prop);
    });

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // --- Add Component Button ---
    float width = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (width - 150.f) * 0.5f);
    if (ImGui::Button("Add Component", ImVec2(150, 30))) {
        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup")) {
        static char searchFilter[128] = "";
        ImGui::InputTextWithHint("##filter", "Search...", searchFilter, sizeof(searchFilter));
        ImGui::Separator();

        for (auto& [typeId, info] : compReg.GetAll()) {
            // Tag(100) は基本必須なので追加リストからは除外
            if (typeId == 100) continue;

            // 既に持っているコンポーネントは表示しない
            if (registry.HasComponent(entity, typeId)) continue;

            // フィルター
            std::string nameLower = info.name;
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            std::string filterLower = searchFilter;
            std::transform(filterLower.begin(), filterLower.end(), filterLower.begin(), ::tolower);

            if (filterLower.empty() || nameLower.find(filterLower) != std::string::npos) {
                if (ImGui::MenuItem(info.name.c_str())) {
                    history.Execute(std::make_shared<AddComponentCommand>(entity, typeId));
                    searchFilter[0] = '\0';
                }
            }
        }
        ImGui::EndPopup();
    }

    EditorUtils::DrawActiveViewOutline();
    ImGui::End();
}

} // namespace Engine::Editor
