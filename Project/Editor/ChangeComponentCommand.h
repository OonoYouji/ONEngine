#pragma once
#include "CommandHistory.h"
#include "Engine/ECS/Registry.h"
#include "Engine/ECS/ComponentRegistry.h"
#include "Engine/Core/Application.h"
#include <nlohmann/json.hpp>

namespace Engine::Editor {

using json = nlohmann::json;

///
/// コンポーネントの値を変更する汎用コマンド
/// JSONシリアライズを利用して状態を保存する
///
class ChangeComponentCommand : public ICommand {
public:
    ChangeComponentCommand(ECS::Entity entity, uint32_t componentId, const json& oldState, const json& newState)
        : entity_(entity), componentId_(componentId), oldState_(oldState), newState_(newState) {}

    void Execute() override {
        ApplyState(newState_);
    }

    void Undo() override {
        ApplyState(oldState_);
    }

    std::string GetName() const override { return "Change Component Value"; }

private:
    void ApplyState(const json& state) {
        auto& compReg = ECS::ComponentRegistry::GetInstance();
        auto& registry = Engine::Core::Application::GetInstance().GetRegistry();
        
        // Registry に直接適用
        compReg.DeserializeComponent(registry, entity_, componentId_, state);
    }

    ECS::Entity entity_;
    uint32_t componentId_;
    json oldState_;
    json newState_;
};

} // namespace Engine::Editor
