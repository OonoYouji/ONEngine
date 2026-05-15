#include "HierarchyView.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "EditorContext.h"
#include "EditorUtils.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"
#include "Engine/Scene/HierarchySystem.h"
#include "Editor/ChangeComponentCommand.h"
#include "Editor/CommandHistory.h"
#include "Engine/ECS/ComponentRegistry.h"
#include <string>
#include <vector>
#include <algorithm>

namespace Engine::Editor {

// 描画された各エンティティの表示領域を保持する（ボックス選択用）
static std::vector<std::pair<ECS::Entity, ImRect>> g_VisibleItemRects;

void HierarchyView::Render(ECS::Registry& registry, bool* p_open) {
    if (p_open && !*p_open) return;
    ImGui::Begin("Hierarchy", p_open);

    auto& io = ImGui::GetIO();
    auto& context = EditorContext::GetInstance();
    
    // --- シーン名の表示 ---
    std::string scenePath = context.GetCurrentScenePath();
    std::string sceneName = "Untitled Scene";
    if (!scenePath.empty()) {
        std::filesystem::path p(scenePath);
        sceneName = p.stem().string();
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f)); // 水色
    ImGui::TextDisabled("Scene:"); ImGui::SameLine();
    ImGui::Text("%s", sceneName.c_str());
    ImGui::PopStyleColor();
    ImGui::Separator();

    auto& transformStorage = registry.GetStorage<ECS::Transform>();
    const auto& entities = transformStorage.GetEntities();

    // 背景クリックで選択解除
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) {
        EditorContext::GetInstance().ClearSelection();
    }

    // 前フレームのRect情報を元にボックス選択を処理（描画前に行う必要があるため）
    static bool isBoxSelecting = false;
    static ImVec2 boxStartPos;

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
        if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) {
            isBoxSelecting = true;
            boxStartPos = io.MousePos;
            if (!io.KeyCtrl) context.ClearSelection();
        }
    }

    if (isBoxSelecting) {
        if (ImGui::IsMouseReleased(0)) {
            isBoxSelecting = false;
        } else {
            ImVec2 mousePos = io.MousePos;
            ImVec2 boxMin = ImVec2((std::min)(boxStartPos.x, mousePos.x), (std::min)(boxStartPos.y, mousePos.y));
            ImVec2 boxMax = ImVec2((std::max)(boxStartPos.x, mousePos.x), (std::max)(boxStartPos.y, mousePos.y));

            ImGui::GetForegroundDrawList()->AddRect(boxMin, boxMax, IM_COL32(100, 150, 255, 255));
            ImGui::GetForegroundDrawList()->AddRectFilled(boxMin, boxMax, IM_COL32(100, 150, 255, 50));

            if (!io.KeyCtrl) context.ClearSelection();
            for (const auto& item : g_VisibleItemRects) {
                if (item.second.Max.x < boxMin.x || item.second.Min.x > boxMax.x ||
                    item.second.Max.y < boxMin.y || item.second.Min.y > boxMax.y) {
                    continue;
                }
                context.AddToSelection(item.first);
            }
        }
    }

    // 今フレームの描画順Rect収集を開始
    g_VisibleItemRects.clear();

    // ルートエンティティの収集
    std::vector<ECS::Entity> rootEntities;
    for (auto entity : entities) {
        if (registry.HasComponent<ECS::Transform>(entity)) {
            auto& transform = registry.GetComponent<ECS::Transform>(entity);
            if (transform.parent == ECS::kNullEntity) {
                rootEntities.push_back(entity);
            }
        }
    }

    std::sort(rootEntities.begin(), rootEntities.end(), [&](ECS::Entity a, ECS::Entity b) {
        float orderA = registry.GetComponent<ECS::Transform>(a).sortOrder;
        float orderB = registry.GetComponent<ECS::Transform>(b).sortOrder;
        if (orderA != orderB) return orderA < orderB;
        return a < b;
    });

    // --- ルート階層の描画 ---
    float firstOrder = rootEntities.empty() ? 0.0f : registry.GetComponent<ECS::Transform>(rootEntities.front()).sortOrder;
    DrawDropSeparator(registry, 0x1000, ECS::kNullEntity, firstOrder - 1.0f, ECS::kNullEntity, rootEntities.empty() ? ECS::kNullEntity : rootEntities.front());

    for (size_t i = 0; i < rootEntities.size(); ++i) {
        ECS::Entity e = rootEntities[i];
        DrawEntityNode(registry, e);
        
        float currentOrder = registry.GetComponent<ECS::Transform>(e).sortOrder;
        float nextOrder = (i + 1 < rootEntities.size()) 
            ? registry.GetComponent<ECS::Transform>(rootEntities[i+1]).sortOrder 
            : currentOrder + 1.0f;
        
        float midOrder = (currentOrder + nextOrder) * 0.5f;
        if (midOrder == currentOrder) midOrder += 0.0001f;

        ECS::Entity nextEnt = (i + 1 < rootEntities.size()) ? rootEntities[i+1] : ECS::kNullEntity;
        DrawDropSeparator(registry, 0x2000 + i, ECS::kNullEntity, midOrder, e, nextEnt);
    }

    // ウィンドウの残りのスペース
    ImGui::Dummy(ImGui::GetContentRegionAvail());
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_ID")) {
            ECS::Entity draggedEntity = *(const ECS::Entity*)payload->Data;
            ReparentEntity(registry, draggedEntity, ECS::kNullEntity);
        }
        ImGui::EndDragDropTarget();
    }

    EditorUtils::DrawActiveViewOutline();
    ImGui::End();
}

void HierarchyView::DrawEntityNode(ECS::Registry& registry, ECS::Entity entity) {
    auto& context = EditorContext::GetInstance();
    
    std::string name = "Entity " + std::to_string(entity);
    if (registry.HasComponent<ECS::Tag>(entity)) {
        name = registry.GetComponent<ECS::Tag>(entity).name;
    }

    ImGuiTreeNodeFlags flags = (context.IsSelected(entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
    
    // 子の収集
    std::vector<ECS::Entity> children;
    auto& transformStorage = registry.GetStorage<ECS::Transform>();
    for (auto e : transformStorage.GetEntities()) {
        if (registry.HasComponent<ECS::Transform>(e) && registry.GetComponent<ECS::Transform>(e).parent == entity) {
            children.push_back(e);
        }
    }

    std::sort(children.begin(), children.end(), [&](ECS::Entity a, ECS::Entity b) {
        float orderA = registry.GetComponent<ECS::Transform>(a).sortOrder;
        float orderB = registry.GetComponent<ECS::Transform>(b).sortOrder;
        if (orderA != orderB) return orderA < orderB;
        return a < b;
    });

    if (children.empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    auto& tag = registry.GetComponent<ECS::Tag>(entity);
    bool isActive = tag.isActive != 0;

    ImGui::PushID((int)entity);
    bool check = isActive;
    if (ImGui::Checkbox("##active", &check)) {
        auto oldState = ECS::ComponentRegistry::GetInstance().SerializeComponent(registry, entity, 100);
        tag.isActive = check ? 1 : 0;
        auto newState = ECS::ComponentRegistry::GetInstance().SerializeComponent(registry, entity, 100);
        CommandHistory::GetInstance().Execute(std::make_shared<ChangeComponentCommand>(entity, 100, oldState, newState));
    }
    ImGui::PopID();
    
    ImGui::SameLine();

    // ノードの描画
    if (!isActive) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, name.c_str());
    if (!isActive) ImGui::PopStyleColor();

    // 描画された領域を記録（ボックス選択用）
    g_VisibleItemRects.push_back({ entity, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()) });

    static ECS::Entity lastSelectedEntity = ECS::kNullEntity;
    auto& io = ImGui::GetIO();

    // 選択
    if (ImGui::IsItemClicked()) {
        if (io.KeyCtrl) {
            if (context.IsSelected(entity)) context.RemoveFromSelection(entity);
            else context.AddToSelection(entity);
        } else {
            context.SetSelectedEntity(entity);
        }
        lastSelectedEntity = entity;
    }

    // ドラッグソース
    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("ENTITY_ID", &entity, sizeof(ECS::Entity));
        ImGui::Text("%s", name.c_str());
        ImGui::EndDragDropSource();
    }

    // ドロップターゲット（親子化）
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_ID")) {
            ECS::Entity draggedEntity = *(const ECS::Entity*)payload->Data;
            if (draggedEntity != entity && !Scene::HierarchySystem::IsDescendantOf(registry, entity, draggedEntity)) {
                ReparentEntity(registry, draggedEntity, entity);
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (opened && !children.empty()) {
        float firstOrder = children.empty() ? 0.0f : registry.GetComponent<ECS::Transform>(children.front()).sortOrder;
        DrawDropSeparator(registry, 0x3000 + (uint32_t)entity, entity, firstOrder - 1.0f, ECS::kNullEntity, children.front());

        for (size_t i = 0; i < children.size(); ++i) {
            ECS::Entity c = children[i];
            DrawEntityNode(registry, c);

            float currentOrder = registry.GetComponent<ECS::Transform>(c).sortOrder;
            float nextOrder = (i + 1 < children.size()) 
                ? registry.GetComponent<ECS::Transform>(children[i+1]).sortOrder 
                : currentOrder + 1.0f;
            
            float midOrder = (currentOrder + nextOrder) * 0.5f;
            if (midOrder == currentOrder) midOrder += 0.0001f;

            ECS::Entity nextEnt = (i + 1 < children.size()) ? children[i+1] : ECS::kNullEntity;
            DrawDropSeparator(registry, 0x4000 + (uint32_t)entity + i, entity, midOrder, c, nextEnt);
        }
        ImGui::TreePop();
    }
}

void HierarchyView::DrawDropSeparator(ECS::Registry& registry, uint32_t id, ECS::Entity parent, float newOrder, ECS::Entity blockedA, ECS::Entity blockedB) {
    float height = 6.0f;
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 0.0f));
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (height * 0.5f));
    
    ImGui::PushID(id);
    ImGui::InvisibleButton("##sep", ImVec2(-1, height));

    if (ImGui::BeginDragDropTarget()) {
        ImRect rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(rect.Min.x, (rect.Min.y + rect.Max.y) * 0.5f),
            ImVec2(rect.Max.x, (rect.Min.y + rect.Max.y) * 0.5f),
            IM_COL32(255, 255, 0, 255), 2.0f);

        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_ID")) {
            ECS::Entity draggedEntity = *(const ECS::Entity*)payload->Data;
            if (draggedEntity != blockedA && draggedEntity != blockedB && draggedEntity != parent) {
                if (!Scene::HierarchySystem::IsDescendantOf(registry, parent, draggedEntity)) {
                    UpdateEntityOrder(registry, draggedEntity, parent, newOrder);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::PopID();
    
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (height * 0.5f));
    ImGui::PopStyleVar();
}

void HierarchyView::UpdateEntityOrder(ECS::Registry& registry, ECS::Entity entity, ECS::Entity newParent, float newOrder) {
    auto& transform = registry.GetComponent<ECS::Transform>(entity);
    json oldState;
    to_json(oldState, transform);
    
    json newState = oldState;
    newState["parent"] = newParent;
    newState["sortOrder"] = newOrder;

    Engine::Console::Log(std::format("[Hierarchy] Changing Entity {} Parent: {} -> {}, Order: {} -> {}", 
        entity, (uint32_t)transform.parent, (uint32_t)newParent, transform.sortOrder, newOrder));

    auto* info = ECS::ComponentRegistry::GetInstance().GetInfo("Transform");
    if (info) {
        CommandHistory::GetInstance().Execute(std::make_shared<ChangeComponentCommand>(entity, info->typeId, oldState, newState));
    }
}

void HierarchyView::ReparentEntity(ECS::Registry& registry, ECS::Entity entity, ECS::Entity newParent) {
    if (entity == newParent) return;
    float maxOrder = -1.0f;
    auto& transformStorage = registry.GetStorage<ECS::Transform>();
    for (auto e : transformStorage.GetEntities()) {
        if (registry.HasComponent<ECS::Transform>(e) && registry.GetComponent<ECS::Transform>(e).parent == newParent) {
            maxOrder = (std::max)(maxOrder, registry.GetComponent<ECS::Transform>(e).sortOrder);
        }
    }
    UpdateEntityOrder(registry, entity, newParent, maxOrder + 1.0f);
}

} // namespace Engine::Editor
