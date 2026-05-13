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

    // 自動生成された UI 関数を使用して描画
    DrawComponent(100, "Tag", [](void* data, auto Prop) {
        ECS::DrawUI_Tag(*static_cast<ECS::Tag*>(data), Prop);
    });

    DrawComponent(1, "Transform", [](void* data, auto Prop) {
        ECS::DrawUI_Transform(*static_cast<ECS::Transform*>(data), Prop);
    });

    DrawComponent(2, "MeshRenderer", [](void* data, auto Prop) {
        ECS::DrawUI_MeshRenderer(*static_cast<ECS::MeshRenderer*>(data), Prop);
    });

    DrawComponent(3, "Camera", [](void* data, auto Prop) {
        ECS::DrawUI_Camera(*static_cast<ECS::Camera*>(data), Prop);
    });

    DrawComponent(4, "DirectionalLight", [](void* data, auto Prop) {
        ECS::DrawUI_DirectionalLight(*static_cast<ECS::DirectionalLight*>(data), Prop);
    });

    DrawComponent(5, "PointLight", [](void* data, auto Prop) {
        ECS::DrawUI_PointLight(*static_cast<ECS::PointLight*>(data), Prop);
    });

    DrawComponent(6, "SpriteRenderer", [](void* data, auto Prop) {
        ECS::DrawUI_SpriteRenderer(*static_cast<ECS::SpriteRenderer*>(data), Prop);
    });

    DrawComponent(7, "ParticleEmitter", [](void* data, auto Prop) {
        ECS::DrawUI_ParticleEmitter(*static_cast<ECS::ParticleEmitter*>(data), Prop);
    });

    DrawComponent(8, "Skybox", [](void* data, auto Prop) {
        ECS::DrawUI_Skybox(*static_cast<ECS::Skybox*>(data), Prop);
    });

    DrawComponent(9, "TextRenderer", [](void* data, auto Prop) {
        ECS::DrawUI_TextRenderer(*static_cast<ECS::TextRenderer*>(data), Prop);
    });

    ImGui::End();
}

} // namespace Engine::Editor
