#include "EditorContext.h"
#include "Engine/Core/Application.h"
#include "Engine/Scene/SceneLoader.h"
#include "Engine/ECS/Registry.h"
#include "Engine/Common/Console.h"
#include "CommandHistory.h"
#include "Schema/Components.h"

namespace Engine::Editor {

bool EditorContext::IsPlaying() const {
    return Engine::Core::Application::GetInstance().IsPlaying();
}

void EditorContext::Play() {
    Engine::Core::Application::GetInstance().Play();
}

void EditorContext::Stop() {
    Engine::Core::Application::GetInstance().Stop();
}

bool EditorContext::IsPaused() const {
    return Engine::Core::Application::GetInstance().IsPaused();
}

void EditorContext::SetPaused(bool paused) {
    Engine::Core::Application::GetInstance().SetPaused(paused);
}

bool EditorContext::IsPrefabMode() const {
    if (activeTabIndex_ < 0 || activeTabIndex_ >= tabs_.size()) return false;
    return tabs_[activeTabIndex_].type == TabType::Prefab;
}

const std::string& EditorContext::GetEditingPrefabPath() const {
    static std::string empty;
    if (activeTabIndex_ < 0 || activeTabIndex_ >= tabs_.size()) return empty;
    return tabs_[activeTabIndex_].path;
}

void EditorContext::EnterPrefabMode(const std::string& prefabPath) {
    OpenPrefabTab(prefabPath);
}

void EditorContext::ExitPrefabMode() {
    // プレハブタブを閉じてシーンタブに戻る
    if (IsPrefabMode()) {
        CloseTab(activeTabIndex_);
    }
}

void EditorContext::SaveEditingPrefab() {
    SaveActiveTab();
}

void EditorContext::SetActiveTab(int index) {
    if (index < 0 || index >= (int)tabs_.size()) {
        Engine::Console::LogError(std::format("EditorContext: SetActiveTab failed - invalid index {}", index));
        return;
    }
    if (index == activeTabIndex_) return;

    Engine::Console::Log(std::format("EditorContext: Switching tab from {} to {}", activeTabIndex_, index));
    SaveTabState(activeTabIndex_);
    activeTabIndex_ = index;
    LoadTabState(activeTabIndex_);
    Engine::Console::Log(std::format("EditorContext: Active tab is now: {}", tabs_[index].name));
}

void EditorContext::OpenPrefabTab(const std::string& path) {
    Engine::Console::Log(std::format("EditorContext: Attempting to open Prefab tab: {}", path));
    
    // 既に開いているかチェック
    for (int i = 0; i < (int)tabs_.size(); ++i) {
        if (tabs_[i].type == TabType::Prefab && tabs_[i].path == path) {
            Engine::Console::Log(std::format("EditorContext: Prefab {} is already open, switching to it.", path));
            SetActiveTab(i);
            return;
        }
    }

    // 新しいタブを追加
    EditorTab newTab;
    newTab.type = TabType::Prefab;
    newTab.path = path;
    newTab.name = std::filesystem::path(path).filename().string();
    
    tabs_.push_back(newTab);
    Engine::Console::Log(std::format("EditorContext: Created new tab for prefab: {}", newTab.name));
    SetActiveTab((int)tabs_.size() - 1);
}

void EditorContext::OpenMaterialTab(const std::string& path) {
    Engine::Console::Log(std::format("EditorContext: Attempting to open Material tab: {}", path));
    
    // 既に開いているかチェック
    for (int i = 0; i < (int)tabs_.size(); ++i) {
        if (tabs_[i].type == TabType::Material && tabs_[i].path == path) {
            Engine::Console::Log(std::format("EditorContext: Material {} is already open, switching to it.", path));
            SetActiveTab(i);
            return;
        }
    }

    // 新しいタブを追加
    EditorTab newTab;
    newTab.type = TabType::Material;
    newTab.path = path;
    newTab.name = std::filesystem::path(path).filename().string();
    
    tabs_.push_back(newTab);
    Engine::Console::Log(std::format("EditorContext: Created new tab for material: {}", newTab.name));
    SetActiveTab((int)tabs_.size() - 1);
}

void EditorContext::CloseTab(int index) {
    if (index < 0 || index >= (int)tabs_.size()) {
        Engine::Console::LogError(std::format("EditorContext: CloseTab failed - invalid index {}", index));
        return;
    }
    if (tabs_[index].type == TabType::Scene) {
        Engine::Console::LogWarning("EditorContext: Cannot close the Scene tab.");
        return; 
    }

    std::string tabName = tabs_[index].name;
    tabs_.erase(tabs_.begin() + index);
    Engine::Console::Log(std::format("EditorContext: Closed tab: {}", tabName));
    
    if (activeTabIndex_ >= index) {
        activeTabIndex_ = (std::max)(0, activeTabIndex_ - 1);
        Engine::Console::Log(std::format("EditorContext: Adjusting active tab to {}", activeTabIndex_));
        LoadTabState(activeTabIndex_);
    }
}

void EditorContext::SaveActiveTab() {
    if (activeTabIndex_ < 0 || activeTabIndex_ >= (int)tabs_.size()) return;
    auto& tab = tabs_[activeTabIndex_];
    auto& registry = Engine::Core::Application::GetInstance().GetRegistry();

    Engine::Console::Log(std::format("EditorContext: Saving active tab: {}", tab.name));

    if (tab.type == TabType::Prefab) {
        auto entity = GetSelectedEntity();
        if (entity == ECS::kNullEntity) {
            auto& tagStorage = registry.GetStorage<ECS::Tag>();
            if (tagStorage.GetEntities().size() > 0) entity = tagStorage.GetEntities()[0];
        }
        if (entity != ECS::kNullEntity) {
            if (Engine::Scene::SceneLoader::SavePrefab(tab.path, entity, registry)) {
                Engine::Console::Log(std::format("EditorContext: Successfully saved prefab to {}", tab.path));
            } else {
                Engine::Console::LogError(std::format("EditorContext: Failed to save prefab to {}", tab.path));
            }
        } else {
            Engine::Console::LogWarning("EditorContext: No entity found to save as prefab.");
        }
    } else {
        if (Engine::Scene::SceneLoader::SaveScene(tab.path, registry)) {
            Engine::Console::Log(std::format("EditorContext: Successfully saved scene to {}", tab.path));
        } else {
            Engine::Console::LogError(std::format("EditorContext: Failed to save scene to {}", tab.path));
        }
    }
    tab.isDirty = false;
}

void EditorContext::SaveTabState(int index) {
    if (index < 0 || index >= (int)tabs_.size()) return;
    // Registry snapshot logic ...
    Engine::Console::Log(std::format("EditorContext: Cached state for tab {}", tabs_[index].name));
}

void EditorContext::LoadTabState(int index) {
    if (index < 0 || index >= (int)tabs_.size()) return;
    auto& tab = tabs_[index];
    auto& registry = Engine::Core::Application::GetInstance().GetRegistry();

    Engine::Console::Log(std::format("EditorContext: Loading state for tab: {} ({})", tab.name, tab.path));
    registry.Clear();
    CommandHistory::GetInstance().Clear();
    ClearSelection();

    if (tab.type == TabType::Scene) {
        if (Engine::Scene::SceneLoader::LoadScene(tab.path, registry)) {
            Engine::Console::Log(std::format("EditorContext: Successfully loaded scene: {}", tab.path));
        } else {
            Engine::Console::LogError(std::format("EditorContext: Failed to load scene: {}", tab.path));
        }
    } else if (tab.type == TabType::Prefab) {
        ECS::Entity e = Engine::Scene::SceneLoader::InstantiatePrefab(tab.path, registry);
        if (e != ECS::kNullEntity) {
            SetSelectedEntity(e);
            Engine::Console::Log(std::format("EditorContext: Successfully instantiated prefab: {}", tab.path));
        } else {
            Engine::Console::LogError(std::format("EditorContext: Failed to instantiate prefab: {}", tab.path));
        }
    } else if (tab.type == TabType::Material) {
        Engine::Console::Log(std::format("EditorContext: Material mode - showing material editor for {}", tab.path));
        // Material mode specific setup (e.g., set selected asset)
        SetSelectedAsset(tab.path);
    }
}

} // namespace Engine::Editor
