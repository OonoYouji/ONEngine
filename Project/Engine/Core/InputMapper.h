#pragma once
#include "Engine/Core/RawInputService.h"
#include "Engine/Core/InputBinding.h"
#include "Engine/Core/InputActions.h"

namespace Engine::Core {

class InputMapper {
public:
    /// @brief 入力マッピングの実行
    /// @param raw 生入力サービス
    /// @param table バインドテーブル
    /// @param outMap 更新対象の ActionMap
    /// @param ignoreCapture ImGui のキャプチャを無視するか（エディタ操作用は true を推奨）
    void Update(const RawInputService& raw, const BindingTable& table, ActionMap& outMap, bool ignoreCapture = false) {
        outMap.BeginFrame();
        
        for (const auto& binding : table.GetBindings()) {
            float value = 0.0f;
            bool pressed = false;

            switch (binding.sourceType) {
            case InputSourceType::Keyboard:
                pressed = ignoreCapture ? raw.IsKeyDownRaw(binding.rawCode) : raw.IsKeyDown(binding.rawCode);
                value = pressed ? 1.0f : 0.0f;
                break;
            case InputSourceType::MouseButton:
                pressed = ignoreCapture ? raw.IsMouseButtonDownRaw(binding.rawCode) : raw.IsMouseButtonDown(binding.rawCode);
                value = pressed ? 1.0f : 0.0f;
                break;
            case InputSourceType::MouseWheel:
                // ホイールはキャプチャを無視する場合が多い（ズームなど）
                value = raw.GetMouseWheel() * binding.multiplier;
                pressed = (value != 0.0f);
                break;
            default:
                break;
            }

            outMap.AccumulateAction(binding.actionName, value, pressed);
        }

        outMap.EndFrame();
    }
};

} // namespace Engine::Core
