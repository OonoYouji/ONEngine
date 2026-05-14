#include "InspectorView.h"
#include "imgui.h"
#include "EditorContext.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"
#include "ChangeComponentCommand.h"
#include "Engine/Core/Application.h"

namespace Engine::Editor {

using json = nlohmann::json;
static json s_oldState;

void InspectorView::Render(ECS::Registry& registry) {
    ImGui::Begin("Inspector");

    auto entity = EditorContext::GetInstance().GetSelectedEntity();
    if (entity == 0) {
        ImGui::Text("No Entity Selected");
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

        if (ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID(typeId);
            void* comp = storage.GetRaw(entity);
            
            auto Property = [&](const char* label, auto widgetFunc) {
                json potentialOldState = compReg.SerializeComponent(registry, entity, typeId);
                
                ImGui::PushID(label);
                bool changed = widgetFunc();

                if (ImGui::IsItemActivated()) {
                    s_oldState = potentialOldState;
                }

                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    json newState = compReg.SerializeComponent(registry, entity, typeId);
                    history.Execute(std::make_shared<ChangeComponentCommand>(entity, typeId, s_oldState, newState));
                }
                ImGui::PopID();
                return changed;
            };

            drawUI(comp, Property);

            ImGui::PopID();
        }
    };

    // ID 100: Tag (Inspector上部の名前編集と重複するため、ここでは非表示にするか、詳細として残す)
    // 今回は上部に名前入力欄を作ったので、Tagコンポーネントのヘッダー自体は出さないように調整
    // (Tagコンポーネントに名前以外のフィールドが増えた場合は CollapsingHeader が必要)

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

    ImGui::End();
}

} // namespace Engine::Editor
