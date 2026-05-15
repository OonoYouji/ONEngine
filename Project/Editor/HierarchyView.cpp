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

void HierarchyView::Render(ECS::Registry& registry) {
    ImGui::Begin("Hierarchy");

    // --- シーン名の表示 ---
    auto& context = EditorContext::GetInstance();
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
        EditorContext::GetInstance().SetSelectedEntity(ECS::kNullEntity);
    }

    // ルートエンティティの収集
    std::vector<ECS::Entity> rootEntities;
    bool anyNonZero = false;
    for (auto entity : entities) {
        if (registry.HasComponent<ECS::Transform>(entity)) {
            auto& transform = registry.GetComponent<ECS::Transform>(entity);
            if (transform.parent == ECS::kNullEntity) {
                rootEntities.push_back(entity);
                if (transform.sortOrder != 0.0f) anyNonZero = true;
            }
        }
    }

    // すべて 0 の場合は初期化（インデックス順にする）
    if (!anyNonZero && !rootEntities.empty()) {
        for (size_t i = 0; i < rootEntities.size(); ++i) {
            registry.GetComponent<ECS::Transform>(rootEntities[i]).sortOrder = (float)i;
        }
    }

    std::sort(rootEntities.begin(), rootEntities.end(), [&](ECS::Entity a, ECS::Entity b) {
        float orderA = registry.GetComponent<ECS::Transform>(a).sortOrder;
        float orderB = registry.GetComponent<ECS::Transform>(b).sortOrder;
        if (orderA != orderB) return orderA < orderB;
        return a < b; // IDで安定させる
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
        
        // 前後の境界と同じにならないよう微調整（精度対策）
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

    ImGuiTreeNodeFlags flags = ((context.GetSelectedEntity() == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
    
    // 子の収集
    std::vector<ECS::Entity> children;
    bool anyNonZero = false;
    auto& transformStorage = registry.GetStorage<ECS::Transform>();
    for (auto e : transformStorage.GetEntities()) {
        if (registry.HasComponent<ECS::Transform>(e) && registry.GetComponent<ECS::Transform>(e).parent == entity) {
            children.push_back(e);
            if (registry.GetComponent<ECS::Transform>(e).sortOrder != 0.0f) anyNonZero = true;
        }
    }

    if (!anyNonZero && !children.empty()) {
        for (size_t i = 0; i < children.size(); ++i) {
            registry.GetComponent<ECS::Transform>(children[i]).sortOrder = (float)i;
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

    // --- Entity Active Checkbox ---
    // Tagコンポーネントがない場合は取得時に作成（isActive管理のため）
    if (!registry.HasComponent<ECS::Tag>(entity)) {
        auto& tag = registry.AddComponent<ECS::Tag>(entity);
        tag.isActive = 1;
        sprintf_s(tag.name, "Entity %u", entity);
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

    // 選択
    if (ImGui::IsItemClicked()) {
        context.SetSelectedEntity(entity);
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
    // 隙間の設定
    // 見た目の隙間を最小化するため、カーソル位置を少し戻してから描画する
    float height = 6.0f;
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 0.0f));
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (height * 0.5f));
    
    ImGui::PushID(id);
    // InvisibleButton は Rect フィードバックを生成する
    ImGui::InvisibleButton("##sep", ImVec2(-1, height));

    if (ImGui::BeginDragDropTarget()) {
        // ドラッグ中のホバー時に黄色い線を描画して挿入位置を明示する
        ImRect rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(rect.Min.x, (rect.Min.y + rect.Max.y) * 0.5f),
            ImVec2(rect.Max.x, (rect.Min.y + rect.Max.y) * 0.5f),
            IM_COL32(255, 255, 0, 255), 2.0f);

        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_ID")) {
            ECS::Entity draggedEntity = *(const ECS::Entity*)payload->Data;
            
            // 自分の現在の位置の前後は無視
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

    auto* info = ECS::ComponentRegistry::GetInstance().GetInfo("Transform");
    if (info) {
        auto command = std::make_shared<ChangeComponentCommand>(
            entity, 
            info->typeId, 
            oldState, 
            newState
        );
        CommandHistory::GetInstance().Execute(command);
    }
}

void HierarchyView::ReparentEntity(ECS::Registry& registry, ECS::Entity entity, ECS::Entity newParent) {
    if (entity == newParent) return;

    auto& transform = registry.GetComponent<ECS::Transform>(entity);
    
    // 末尾に追加するためのsortOrderを決定
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
