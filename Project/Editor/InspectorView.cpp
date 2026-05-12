#include "InspectorView.h"
#include "imgui.h"
#include "EditorContext.h"
#include "Schema/Schema.h"

namespace Engine::Editor {

void InspectorView::Render(ECS::Registry& registry) {
    ImGui::Begin("Inspector");

    auto entity = EditorContext::GetInstance().GetSelectedEntity();
    if (entity == 0) {
        ImGui::Text("No Entity Selected");
        ImGui::End();
        return;
    }

    // Tag Component
    if (registry.HasComponent<ECS::Tag>(entity)) {
        if (ImGui::CollapsingHeader("Tag", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto& tag = registry.GetComponent<ECS::Tag>(entity);
            ImGui::InputText("Name", tag.name, sizeof(tag.name));
        }
    }

    // Transform Component
    if (registry.HasComponent<ECS::Transform>(entity)) {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto& transform = registry.GetComponent<ECS::Transform>(entity);
            ImGui::DragFloat3("Position", &transform.position.x, 0.1f);
            ImGui::DragFloat3("Rotation", &transform.rotation.x, 0.1f);
            ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
        }
    }

    // MeshRenderer Component
    if (registry.HasComponent<ECS::MeshRenderer>(entity)) {
        if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto& mr = registry.GetComponent<ECS::MeshRenderer>(entity);
            ImGui::InputScalar("Model Index", ImGuiDataType_U32, &mr.modelIndex);
            ImGui::InputScalar("Material Index", ImGuiDataType_U32, &mr.materialIndex);
        }
    }

    // Camera Component
    if (registry.HasComponent<ECS::Camera>(entity)) {
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto& camera = registry.GetComponent<ECS::Camera>(entity);
            ImGui::DragFloat("FOV", &camera.fov, 1.0f, 1.0f, 179.0f);
            ImGui::DragFloat("Near Z", &camera.nearZ, 0.1f, 0.01f, 10.0f);
            ImGui::DragFloat("Far Z", &camera.farZ, 10.0f, 10.0f, 10000.0f);
        }
    }

    // DirectionalLight Component
    if (registry.HasComponent<ECS::DirectionalLight>(entity)) {
        if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto& light = registry.GetComponent<ECS::DirectionalLight>(entity);
            ImGui::ColorEdit3("Color", &light.color.x);
            ImGui::DragFloat("Intensity", &light.intensity, 0.1f, 0.0f, 100.0f);
            ImGui::DragFloat3("Direction", &light.direction.x, 0.01f);
        }
    }

    ImGui::End();
}

} // namespace Engine::Editor
