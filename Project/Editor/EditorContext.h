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

namespace Engine::Editor {

namespace ECS = Engine::ECS;
namespace Core = Engine::Core;

class EditorContext {
public:
    static EditorContext& GetInstance() {
        static EditorContext instance;
        return instance;
    }

    void SetSelectedEntity(ECS::Entity entity) { selectedEntity_ = entity; }
    ECS::Entity GetSelectedEntity() const { return selectedEntity_; }

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
        }
    }

private:
    EditorContext() : selectedEntity_(0), snapEnabled_(false), snapTranslation_(1.0f), snapRotation_(45.0f), snapScale_(0.5f), currentScenePath_("Assets/Scene/MainScene.scene") {
        camera_ = std::make_unique<EditorCamera>();
        
        // エディター用のデフォルトバインド
        bindingTable_.AddBinding({ "MoveForward",  Core::InputSourceType::Keyboard, 'W' });
        bindingTable_.AddBinding({ "MoveBackward", Core::InputSourceType::Keyboard, 'S' });
        bindingTable_.AddBinding({ "MoveLeft",     Core::InputSourceType::Keyboard, 'A' });
        bindingTable_.AddBinding({ "MoveRight",    Core::InputSourceType::Keyboard, 'D' });
        bindingTable_.AddBinding({ "MoveUp",       Core::InputSourceType::Keyboard, 'E' });
        bindingTable_.AddBinding({ "MoveDown",     Core::InputSourceType::Keyboard, 'Q' });
        bindingTable_.AddBinding({ "SpeedUp",      Core::InputSourceType::Keyboard, VK_SHIFT });
        bindingTable_.AddBinding({ "Rotate",       Core::InputSourceType::MouseButton, 1 }); // Right Click
        bindingTable_.AddBinding({ "Zoom",         Core::InputSourceType::MouseWheel, 0, 1.0f });

        LoadSettings();
    }
    ECS::Entity selectedEntity_;
    std::unique_ptr<EditorCamera> camera_;
    
    Core::ActionMap actionMap_;
    Core::BindingTable bindingTable_;
    Core::InputMapper inputMapper_;

    bool snapEnabled_;
    float snapTranslation_;
    float snapRotation_;
    float snapScale_;

    std::string currentScenePath_;
};

} // namespace Engine::Editor
