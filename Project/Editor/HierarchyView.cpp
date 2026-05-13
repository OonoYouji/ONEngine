#include "HierarchyView.h"
#include "imgui.h"
#include "EditorContext.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"
#include <string>

namespace Engine::Editor {

void HierarchyView::Render(ECS::Registry& registry) {
    ImGui::Begin("Hierarchy");

    auto& transformStorage = registry.GetStorage<ECS::Transform>();
    const auto& entities = transformStorage.GetEntities();

    for (auto entity : entities) {
        auto& transform = registry.GetComponent<ECS::Transform>(entity);
        // ルートエンティティ (親がいない、または自分自身が親（通常は0）) のみを描画開始
        if (transform.parent == 0) {
            DrawEntityNode(registry, entity);
        }
    }

    ImGui::End();
}

void HierarchyView::DrawEntityNode(ECS::Registry& registry, ECS::Entity entity) {
    auto& context = EditorContext::GetInstance();
    
    std::string name = "Entity " + std::to_string(entity);
    if (registry.HasComponent<ECS::Tag>(entity)) {
        name = registry.GetComponent<ECS::Tag>(entity).name;
    }

    ImGuiTreeNodeFlags flags = ((context.GetSelectedEntity() == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
    
    // 子がいるかどうかを確認
    bool hasChildren = false;
    auto& transformStorage = registry.GetStorage<ECS::Transform>();
    const auto& entities = transformStorage.GetEntities();
    for (auto e : entities) {
        if (registry.GetComponent<ECS::Transform>(e).parent == entity) {
            hasChildren = true;
            break;
        }
    }

    if (!hasChildren) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, name.c_str());

    if (ImGui::IsItemClicked()) {
        context.SetSelectedEntity(entity);
    }

    if (opened && hasChildren) {
        for (auto e : entities) {
            if (registry.GetComponent<ECS::Transform>(e).parent == entity) {
                DrawEntityNode(registry, e);
            }
        }
        ImGui::TreePop();
    }
}

} // namespace Engine::Editor
