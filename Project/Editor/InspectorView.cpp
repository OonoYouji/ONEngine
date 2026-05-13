#include "InspectorView.h"
#include "imgui.h"
#include "EditorContext.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"
#include "ChangeComponentCommand.h"
#include "Engine/Core/Application.h"

namespace Engine::Editor {

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
            
            // 各ウィジェットをラップして Activation を監視するヘルパー
            auto Property = [&](const char* label, auto widgetFunc) {
                // 変更前の状態をあらかじめシリアライズしておく（Activated の瞬間に使うため）
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

    // Tag
    DrawComponent(100, "Tag", [&](void* data, auto Prop) {
        auto& tag = *static_cast<ECS::Tag*>(data);
        Prop("Name", [&]() { return ImGui::InputText("Name", tag.name, sizeof(tag.name)); });
    });

    // Transform
    DrawComponent(1, "Transform", [&](void* data, auto Prop) {
        auto& t = *static_cast<ECS::Transform*>(data);
        Prop("Position", [&]() { return ImGui::DragFloat3("Position", &t.position.x, 0.1f); });
        Prop("Rotation", [&]() { return ImGui::DragFloat3("Rotation", &t.rotation.x, 0.1f); });
        Prop("Scale",    [&]() { return ImGui::DragFloat3("Scale", &t.scale.x, 0.1f); });
        ImGui::Text("Parent: %u", t.parent);
    });

    // MeshRenderer
    DrawComponent(2, "MeshRenderer", [&](void* data, auto Prop) {
        auto& m = *static_cast<ECS::MeshRenderer*>(data);
        Prop("Model", [&]() { return ImGui::InputScalar("Model Index", ImGuiDataType_U32, &m.modelIndex); });
        Prop("Material", [&]() { return ImGui::InputScalar("Material Index", ImGuiDataType_U32, &m.materialIndex); });
        Prop("PostProcess", [&]() { return ImGui::InputScalar("PostProcess Flags", ImGuiDataType_U32, &m.postProcessFlags); });
    });

    // Camera
    DrawComponent(3, "Camera", [&](void* data, auto Prop) {
        auto& c = *static_cast<ECS::Camera*>(data);
        Prop("FOV", [&]() { return ImGui::DragFloat("FOV", &c.fov, 1.0f, 1.0f, 179.0f); });
        Prop("Near", [&]() { return ImGui::DragFloat("Near Z", &c.nearZ, 0.1f, 0.01f, 10.0f); });
        Prop("Far", [&]() { return ImGui::DragFloat("Far Z", &c.farZ, 10.0f, 10.0f, 10000.0f); });
    });

    // DirectionalLight
    DrawComponent(4, "DirectionalLight", [&](void* data, auto Prop) {
        auto& l = *static_cast<ECS::DirectionalLight*>(data);
        Prop("Color", [&]() { return ImGui::ColorEdit3("Color", &l.color.x); });
        Prop("Intensity", [&]() { return ImGui::DragFloat("Intensity", &l.intensity, 0.1f, 0.0f, 100.0f); });
        Prop("Direction", [&]() { return ImGui::DragFloat3("Direction", &l.direction.x, 0.01f); });
    });

    // SpriteRenderer
    DrawComponent(6, "SpriteRenderer", [&](void* data, auto Prop) {
        auto& s = *static_cast<ECS::SpriteRenderer*>(data);
        Prop("Texture", [&]() { return ImGui::InputScalar("Texture Index", ImGuiDataType_U32, &s.textureIndex); });
        Prop("Color", [&]() { return ImGui::ColorEdit4("Color", &s.color.x); });
        Prop("Size", [&]() { return ImGui::DragFloat2("Size", &s.size.x, 0.1f); });
        Prop("Billboard", [&]() { return ImGui::Checkbox("Billboard", (bool*)&s.isBillboard); });
    });

    ImGui::End();
}

} // namespace Engine::Editor
