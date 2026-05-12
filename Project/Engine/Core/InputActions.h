#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>
#include "Engine/Core/Math/Math.h"

namespace Engine::Core {

struct InputActionState {
    bool isPressed = false;
    bool wasPressed = false;
    bool isTriggered = false;
    bool isReleased = false;
    float value = 0.0f;
};

class ActionMap {
public:
    /// @brief フレームの開始時に状態をリセット
    void BeginFrame() {
        for (auto& [name, state] : actions_) {
            state.wasPressed = state.isPressed;
            state.isPressed = false;
            state.value = 0.0f;
            state.isTriggered = false;
            state.isReleased = false;
        }
    }

    /// @brief 入力を蓄積（複数のバインドがある場合に OR / MAX をとる）
    void AccumulateAction(const std::string& name, float value, bool pressed) {
        auto& state = actions_[name];
        state.isPressed |= pressed;
        if (std::abs(value) > std::abs(state.value)) {
            state.value = value;
        }
    }

    /// @brief 全ての入力蓄積が終わった後にトリガー状態を確定
    void EndFrame() {
        for (auto& [name, state] : actions_) {
            state.isTriggered = !state.wasPressed && state.isPressed;
            state.isReleased = state.wasPressed && !state.isPressed;
        }
    }

    bool IsPressed(const std::string& name) const {
        auto it = actions_.find(name);
        return it != actions_.end() ? it->second.isPressed : false;
    }

    bool IsTriggered(const std::string& name) const {
        auto it = actions_.find(name);
        return it != actions_.end() ? it->second.isTriggered : false;
    }

    float GetValue(const std::string& name) const {
        auto it = actions_.find(name);
        return it != actions_.end() ? it->second.value : 0.0f;
    }

private:
    std::unordered_map<std::string, InputActionState> actions_;
};

} // namespace Engine::Core
