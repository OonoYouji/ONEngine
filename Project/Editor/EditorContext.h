#pragma once
#include "Engine/ECS/Entity.h"
#include "Editor/EditorCamera.h"
#include "Engine/Core/InputActions.h"
#include "Engine/Core/InputBinding.h"
#include "Engine/Core/InputMapper.h"
#include "Externals/nlohmann/json.hpp"
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>

#include <set>
#include <algorithm>

namespace Engine::Editor {

namespace ECS = Engine::ECS;
namespace Core = Engine::Core;

class EditorContext {
public:
    static EditorContext& GetInstance() {
        static EditorContext instance;
        return instance;
    }

    void SetSelectedEntity(ECS::Entity entity) { 
        selection_.clear(); 
        if (entity != 0) {
            selection_.insert(entity); 
            selectedAssetPath_ = "";
        }
    }
    ECS::Entity GetSelectedEntity() const { 
        return selection_.empty() ? 0 : *selection_.begin(); 
    }

    void SetSelectedAsset(const std::filesystem::path& path) {
        selectedAssetPath_ = path;
        if (!selectedAssetPath_.empty()) {
            selection_.clear();
        }
    }
    const std::filesystem::path& GetSelectedAsset() const { return selectedAssetPath_; }

    const std::set<ECS::Entity>& GetSelection() const { return selection_; }
    void AddToSelection(ECS::Entity entity) { if (entity != 0) selection_.insert(entity); }
    void RemoveFromSelection(ECS::Entity entity) { selection_.erase(entity); }
    void ClearSelection() { selection_.clear(); }
    bool IsSelected(ECS::Entity entity) const { return selection_.count(entity) > 0; }

    EditorCamera& GetCamera() { return *camera_; }
    Core::ActionMap& GetActionMap() { return actionMap_; }
    Core::BindingTable& GetBindingTable() { return bindingTable_; }
    Core::InputMapper& GetInputMapper() { return inputMapper_; }

    // Gizmo スナップ設定
    bool& GetSnapEnabled() { return snapEnabled_; }
    float& GetSnapTranslation() { return snapTranslation_; }
    float& GetSnapRotation() { return snapRotation_; }
    float& GetSnapScale() { return snapScale_; }

    void SetCurrentScenePath(const std::string& path) { 
        currentScenePath_ = path; 
        SaveSettings();
    }
    const std::string& GetCurrentScenePath() const { return currentScenePath_; }

    void SaveSettings() {
        nlohmann::json j;
        j["CurrentScenePath"] = currentScenePath_;
        j["SnapEnabled"] = snapEnabled_;
        j["SnapTranslation"] = snapTranslation_;
        j["SnapRotation"] = snapRotation_;
        j["SnapScale"] = snapScale_;
        j["ShowHierarchy"] = showHierarchy_;
        j["ShowInspector"] = showInspector_;
        j["ShowSceneView"] = showSceneView_;
        j["ShowProject"] = showProject_;
        j["ShowConsole"] = showConsole_;

        std::ofstream os("EditorSettings.json");
        if (os.is_open()) {
            os << j.dump(4);
        }
    }

    void LoadSettings() {
        if (!std::filesystem::exists("EditorSettings.json")) {
            return;
        }

        std::ifstream is("EditorSettings.json");
        if (is.is_open()) {
            nlohmann::json j;
            is >> j;

            if (j.contains("CurrentScenePath")) currentScenePath_ = j["CurrentScenePath"];
            if (j.contains("SnapEnabled")) snapEnabled_ = j["SnapEnabled"];
            if (j.contains("SnapTranslation")) snapTranslation_ = j["SnapTranslation"];
            if (j.contains("SnapRotation")) snapRotation_ = j["SnapRotation"];
            if (j.contains("SnapScale")) snapScale_ = j["SnapScale"];
            if (j.contains("ShowHierarchy")) showHierarchy_ = j["ShowHierarchy"];
            if (j.contains("ShowInspector")) showInspector_ = j["ShowInspector"];
            if (j.contains("ShowSceneView")) showSceneView_ = j["ShowSceneView"];
            if (j.contains("ShowProject")) showProject_ = j["ShowProject"];
            if (j.contains("ShowConsole")) showConsole_ = j["ShowConsole"];
        }
    }

    void SetSceneFocused(bool focused) { isSceneFocused_ = focused; }
    bool IsSceneFocused() const { return isSceneFocused_; }

    bool& GetShowHierarchy() { return showHierarchy_; }
    bool& GetShowInspector() { return showInspector_; }
    bool& GetShowSceneView() { return showSceneView_; }
    bool& GetShowProject() { return showProject_; }
    bool& GetShowConsole() { return showConsole_; }

private:
    EditorContext() : 
        snapEnabled_(false), snapTranslation_(1.0f), snapRotation_(45.0f), snapScale_(0.5f), 
        currentScenePath_("Assets/Scene/MainScene.scene"), isSceneFocused_(false),
        showHierarchy_(true), showInspector_(true), showSceneView_(true), showProject_(true), showConsole_(true) 
    {
        camera_ = std::make_unique<EditorCamera>();
        selection_.clear();
        // エディター用のデフォルトバインド
        bindingTable_.AddBinding({ "MoveForward",  Core::InputSourceType::Keyboard, 'W', 1.0f, false, false, false });
        bindingTable_.AddBinding({ "MoveBackward", Core::InputSourceType::Keyboard, 'S', 1.0f, false, false, false });
        bindingTable_.AddBinding({ "MoveLeft",     Core::InputSourceType::Keyboard, 'A', 1.0f, false, false, false });
        bindingTable_.AddBinding({ "MoveRight",    Core::InputSourceType::Keyboard, 'D', 1.0f, false, false, false });
        bindingTable_.AddBinding({ "MoveUp",       Core::InputSourceType::Keyboard, 'E', 1.0f, false, false, false });
        bindingTable_.AddBinding({ "MoveDown",     Core::InputSourceType::Keyboard, 'Q', 1.0f, false, false, false });
        bindingTable_.AddBinding({ "SpeedUp",      Core::InputSourceType::Keyboard, VK_SHIFT, 1.0f, false, false, false });
        bindingTable_.AddBinding({ "Rotate",       Core::InputSourceType::MouseButton, 1, 1.0f, false, false, false }); // Right Click
        bindingTable_.AddBinding({ "Zoom",         Core::InputSourceType::MouseWheel, 0, 1.0f, false, false, false });
        bindingTable_.AddBinding({ "Undo",         Core::InputSourceType::Keyboard, 'Z', 1.0f, true,  false, false }); // Ctrl+Z
        bindingTable_.AddBinding({ "Redo",         Core::InputSourceType::Keyboard, 'Y', 1.0f, true,  false, false }); // Ctrl+Y

        LoadSettings();
    }
    std::set<ECS::Entity> selection_;
    std::unique_ptr<EditorCamera> camera_;
    
    Core::ActionMap actionMap_;
    Core::BindingTable bindingTable_;
    Core::InputMapper inputMapper_;

    bool snapEnabled_;
    float snapTranslation_;
    float snapRotation_;
    float snapScale_;
    bool isSceneFocused_;

    std::string currentScenePath_;
    std::filesystem::path selectedAssetPath_;

    bool showHierarchy_;
    bool showInspector_;
    bool showSceneView_;
    bool showProject_;
    bool showConsole_;
};

} // namespace Engine::Editor
