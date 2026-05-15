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
#include <algorithm>
#include <iterator>

namespace Engine::Editor {

using json = nlohmann::json;
static json s_oldState;

void InspectorView::Render(ECS::Registry& registry, bool* p_open) {
    if (p_open && !*p_open) return;
    ImGui::Begin("Inspector", p_open);

    auto& context = EditorContext::GetInstance();
    const auto& selection = context.GetSelection();

    if (selection.empty()) {
        ImGui::Text("No Entity Selected");
        EditorUtils::DrawActiveViewOutline();
        ImGui::End();
        return;
    }

    auto& compReg = ECS::ComponentRegistry::GetInstance();
    auto& history = CommandHistory::GetInstance();

    // --- Common Components Analysis ---
    std::vector<uint32_t> commonComponentIds;
    bool first = true;
    for (auto entity : selection) {
        std::vector<uint32_t> currentEntityComponents;
        for (auto& [typeId, info] : compReg.GetAll()) {
            if (registry.HasComponent(entity, typeId)) {
                currentEntityComponents.push_back(typeId);
            }
        }
        std::sort(currentEntityComponents.begin(), currentEntityComponents.end());

        if (first) {
            commonComponentIds = currentEntityComponents;
            first = false;
        } else {
            std::vector<uint32_t> intersection;
            std::set_intersection(commonComponentIds.begin(), commonComponentIds.end(),
                                  currentEntityComponents.begin(), currentEntityComponents.end(),
                                  std::back_inserter(intersection));
            commonComponentIds = intersection;
        }
    }

    // Undo/Redo ボタン
    ImGui::BeginDisabled(!history.CanUndo());
    if (ImGui::Button("Undo")) history.Undo();
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(!history.CanRedo());
    if (ImGui::Button("Redo")) history.Redo();
    ImGui::EndDisabled();
    
    ImGui::Separator();

    if (selection.size() == 1) {
        auto entity = *selection.begin();
        ImGui::Text("Entity ID: %u", entity);
        // Tag/Active 編集（単体時のみ）
        auto& tag = registry.GetComponent<ECS::Tag>(entity);
        bool active = tag.isActive != 0;
        if (ImGui::Checkbox("##EntityActive", &active)) {
            auto oldState = compReg.SerializeComponent(registry, entity, 100);
            tag.isActive = active ? 1 : 0;
            auto newState = compReg.SerializeComponent(registry, entity, 100);
            history.Execute(std::make_shared<ChangeComponentCommand>(entity, 100, oldState, newState));
        }
        ImGui::SameLine();
        char buffer[256];
        strcpy_s(buffer, tag.name);
        ImGui::PushItemWidth(-1);
        if (ImGui::InputText("##EntityName", buffer, sizeof(buffer), ImGuiInputTextFlags_AutoSelectAll)) {
            strcpy_s(tag.name, buffer);
        }
        if (ImGui::IsItemActivated()) {
            s_oldState = compReg.SerializeComponent(registry, entity, 100);
        }
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            json newState = compReg.SerializeComponent(registry, entity, 100);
            history.Execute(std::make_shared<ChangeComponentCommand>(entity, 100, s_oldState, newState));
        }
        ImGui::PopItemWidth();
    } else {
        ImGui::Text("%zu entities selected", selection.size());
    }

    ImGui::Separator();

    static std::vector<ChangeComponentCommand::EntityState> s_batchOldStates;

    auto DrawComponent = [&](uint32_t typeId, const char* name, auto drawUI) {
        auto* info = compReg.GetInfo(typeId);
        if (!info) return;

        bool open = true;
        if (ImGui::CollapsingHeader(name, &open, ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID(typeId);
            
            // 複数選択時は最初のエンティティの値を代表として表示
            void* primaryComp = registry.GetComponent(*selection.begin(), typeId);
            if (!primaryComp) {
                ImGui::PopID();
                return;
            }
            
            auto Property = [&](const char* label, auto widgetFunc) {
                ImGui::PushID(label);

                // widgetFunc を呼ぶ「前」に全エンティティの状態を確保しておく（単発変更対策）
                std::vector<ChangeComponentCommand::EntityState> potentialOldStates;
                for (auto entity : selection) {
                    potentialOldStates.push_back({ entity, compReg.SerializeComponent(registry, entity, typeId), {} });
                }

                bool changed = widgetFunc();

                if (ImGui::IsItemActivated()) {
                    s_batchOldStates = potentialOldStates;
                }

                if (ImGui::IsItemDeactivatedAfterEdit() || (changed && !ImGui::IsItemActive())) {
                    std::vector<ChangeComponentCommand::EntityState> finalStates;
                    
                    // ドラッグ＆ドロップ等の単発変更時は、widgetFunc を呼ぶ前に撮った potentialOldStates を使用
                    auto& oldStatesSource = (changed && !ImGui::IsItemActive()) ? potentialOldStates : s_batchOldStates;

                    // 代表(primaryComp)の変更内容を他の全エンティティに同期させる
                    json primaryJson = compReg.SerializeComponent(registry, *selection.begin(), typeId);
                    
                    for (auto& os : oldStatesSource) {
                        finalStates.push_back({ os.entity, os.oldState, primaryJson });
                    }
                    
                    history.Execute(std::make_shared<ChangeComponentCommand>(typeId, finalStates));
                }
                ImGui::PopID();
                return changed;
            };

            drawUI(primaryComp, Property);
            ImGui::PopID();
        }
    };

    // 共通コンポーネントのみ描画
    for (uint32_t typeId : commonComponentIds) {
        auto* info = compReg.GetInfo(typeId);
        if (!info || !info->uiFunc) continue;
        
        // Tag(100) は上部で処理済み
        if (typeId == 100) continue;

        DrawComponent(typeId, info->name.c_str(), [&](void* data, auto Prop) {
            ECS::PropertyFunc wrapper = [&](const char* label, std::function<bool()> widget) {
                return Prop(label, widget);
            };
            info->uiFunc(data, wrapper);
        });
    }

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

        auto primaryEntity = *selection.begin();
        for (auto& [typeId, info] : compReg.GetAll()) {
            // Tag(100) は基本必須なので追加リストからは除外
            if (typeId == 100) continue;

            // 既に持っているコンポーネントは表示しない
            if (registry.HasComponent(primaryEntity, typeId)) continue;

            // フィルター
            std::string nameLower = info.name;
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            std::string filterLower = searchFilter;
            std::transform(filterLower.begin(), filterLower.end(), filterLower.begin(), ::tolower);

            if (filterLower.empty() || nameLower.find(filterLower) != std::string::npos) {
                if (ImGui::MenuItem(info.name.c_str())) {
                    // 選択中の全てのエンティティに追加
                    for (auto e : selection) {
                        if (!registry.HasComponent(e, typeId)) {
                            history.Execute(std::make_shared<AddComponentCommand>(e, typeId));
                        }
                    }
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
