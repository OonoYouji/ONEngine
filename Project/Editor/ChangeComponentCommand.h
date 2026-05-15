#pragma once
#include "CommandHistory.h"
#include "Engine/ECS/Registry.h"
#include "Engine/ECS/ComponentRegistry.h"
#include "Engine/Core/Application.h"
#include <nlohmann/json.hpp>
#include <vector>

namespace Engine::Editor {

namespace ECS = Engine::ECS;
using json = nlohmann::json;

///
/// コンポーネントの値を変更する汎用コマンド
/// JSONシリアライズを利用して状態を保存する
///
class ChangeComponentCommand : public ICommand {
public:
    struct EntityState {
        ECS::Entity entity;
        json oldState;
        json newState;
    };

    ChangeComponentCommand(ECS::Entity entity, uint32_t componentId, const json& oldState, const json& newState)
        : componentId_(componentId) 
    {
        states_.push_back({ entity, oldState, newState });
    }

    ChangeComponentCommand(uint32_t componentId, const std::vector<EntityState>& states)
        : componentId_(componentId), states_(states)
    {}

    void Execute() override {
        Apply(true);
    }

    void Undo() override {
        Apply(false);
    }

    std::string GetName() const override { return "Change Component Value (Batch)"; }

private:
    void Apply(bool isForward) {
        auto& compReg = ECS::ComponentRegistry::GetInstance();
        auto& registry = Engine::Core::Application::GetInstance().GetRegistry();
        
        for (auto& s : states_) {
            const json& state = isForward ? s.newState : s.oldState;
            Engine::Console::Log(std::format("[Command] Entity {}: Applying State: {}", s.entity, state.dump()));
            compReg.DeserializeComponent(registry, s.entity, componentId_, state);
        }
    }

    uint32_t componentId_;
    std::vector<EntityState> states_;
};

} // namespace Engine::Editor
