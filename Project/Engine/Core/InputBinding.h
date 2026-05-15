#pragma once
#include <vector>
#include <string>
#include <variant>
#include <Windows.h>

namespace Engine::Core {

///
/// 物理的な入力ソースの定義
///
enum class InputSourceType {
    Keyboard,
    MouseButton,
    MouseWheel,
    GamepadButton,
    GamepadAxis
};

struct InputBinding {
    std::string actionName;
    InputSourceType sourceType;
    uint32_t rawCode; // VK_コード、マウスボタン、軸インデックス等
    float multiplier = 1.0f;
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
};

///
/// キーバインドの集合
/// JSON 等からロード可能な設計にする
///
class BindingTable {
public:
    void AddBinding(const InputBinding& binding) {
        bindings_.push_back(binding);
    }

    const std::vector<InputBinding>& GetBindings() const { return bindings_; }

    void Clear() { bindings_.clear(); }

private:
    std::vector<InputBinding> bindings_;
};

} // namespace Engine::Core
