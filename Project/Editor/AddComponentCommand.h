#pragma once
#include "CommandHistory.h"
#include "Engine/ECS/Registry.h"
#include "Engine/ECS/ComponentRegistry.h"
#include "Engine/Core/Application.h"
#include <nlohmann/json.hpp>

namespace Engine::Editor {

using json = nlohmann::json;

///
/// コンポーネントを追加・削除するコマンド
///
class AddComponentCommand : public ICommand {
public:
    AddComponentCommand(ECS::Entity entity, uint32_t componentId)
        : entity_(entity), componentId_(componentId) {}

    void Execute() override {
        auto& compReg = ECS::ComponentRegistry::GetInstance();
        auto& registry = Engine::Core::Application::GetInstance().GetRegistry();
        
        auto info = compReg.GetInfo(componentId_);
        if (info) {
            info->addFunc(registry, entity_);
        }
    }

    void Undo() override {
        auto& compReg = ECS::ComponentRegistry::GetInstance();
        auto& registry = Engine::Core::Application::GetInstance().GetRegistry();
        
        auto info = compReg.GetInfo(componentId_);
        if (info) {
            info->getStorageFunc(registry).Remove(entity_);
        }
    }

    std::string GetName() const override { return "Add Component"; }

private:
    ECS::Entity entity_;
    uint32_t componentId_;
};

} // namespace Engine::Editor
