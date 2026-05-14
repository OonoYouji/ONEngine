#pragma once
#include "CommandHistory.h"
#include "Engine/ECS/Registry.h"
#include "Engine/ECS/ComponentRegistry.h"
#include "Engine/Core/Application.h"
#include <nlohmann/json.hpp>

namespace Engine::Editor {

using json = nlohmann::json;

///
/// コンポーネントを削除・復元するコマンド
/// 削除前の状態をJSONで保存し、Undo時に完全復元する
///
class RemoveComponentCommand : public ICommand {
public:
    RemoveComponentCommand(ECS::Entity entity, uint32_t componentId)
        : entity_(entity), componentId_(componentId) {
        
        auto& compReg = ECS::ComponentRegistry::GetInstance();
        auto& registry = Engine::Core::Application::GetInstance().GetRegistry();
        
        // 削除前の状態をバックアップ
        savedState_ = compReg.SerializeComponent(registry, entity_, componentId_);
    }

    void Execute() override {
        auto& compReg = ECS::ComponentRegistry::GetInstance();
        auto& registry = Engine::Core::Application::GetInstance().GetRegistry();
        
        auto info = compReg.GetInfo(componentId_);
        if (info) {
            info->getStorageFunc(registry).Remove(entity_);
        }
    }

    void Undo() override {
        auto& compReg = ECS::ComponentRegistry::GetInstance();
        auto& registry = Engine::Core::Application::GetInstance().GetRegistry();
        
        // バックアップから復元（DeserializeComponent内でAddComponentも行われる）
        compReg.DeserializeComponent(registry, entity_, componentId_, savedState_);
    }

    std::string GetName() const override { return "Remove Component"; }

private:
    ECS::Entity entity_;
    uint32_t componentId_;
    json savedState_;
};

} // namespace Engine::Editor
