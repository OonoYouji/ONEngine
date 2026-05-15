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
        
        bool isCtrlDown = ignoreCapture ? (raw.IsKeyDownRaw(VK_CONTROL) || raw.IsKeyDownRaw(VK_LCONTROL) || raw.IsKeyDownRaw(VK_RCONTROL)) 
                                      : (raw.IsKeyDown(VK_CONTROL) || raw.IsKeyDown(VK_LCONTROL) || raw.IsKeyDown(VK_RCONTROL));
        bool isShiftDown = ignoreCapture ? (raw.IsKeyDownRaw(VK_SHIFT) || raw.IsKeyDownRaw(VK_LSHIFT) || raw.IsKeyDownRaw(VK_RSHIFT)) 
                                       : (raw.IsKeyDown(VK_SHIFT) || raw.IsKeyDown(VK_LSHIFT) || raw.IsKeyDown(VK_RSHIFT));
        bool isAltDown = ignoreCapture ? (raw.IsKeyDownRaw(VK_MENU) || raw.IsKeyDownRaw(VK_LMENU) || raw.IsKeyDownRaw(VK_RMENU)) 
                                     : (raw.IsKeyDown(VK_MENU) || raw.IsKeyDown(VK_LMENU) || raw.IsKeyDown(VK_RMENU));

        for (const auto& binding : table.GetBindings()) {
            float value = 0.0f;
            bool pressed = false;

            // 修飾キーの条件チェック
            bool modifierMatch = true;
            if (binding.ctrl && !isCtrlDown) modifierMatch = false;
            if (binding.shift && !isShiftDown) modifierMatch = false;
            if (binding.alt && !isAltDown) modifierMatch = false;

            if (modifierMatch) {
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
                    value = raw.GetMouseWheel() * binding.multiplier;
                    pressed = (value != 0.0f);
                    break;
                default:
                    break;
                }
            }

            outMap.AccumulateAction(binding.actionName, value, pressed);
        }

        outMap.EndFrame();
    }
};

} // namespace Engine::Core
