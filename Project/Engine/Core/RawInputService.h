#pragma once
#include <array>
#include <Windows.h>
#include "Engine/Core/Math/Math.h"

namespace Engine::Core {

class RawInputService {
public:
    void Update(HWND hwnd);

    // Filtered (ImGui Capture 考慮)
    bool IsKeyDown(uint32_t vk) const { return !wantCaptureKeyboard_ && keys_[vk]; }
    bool IsMouseButtonDown(uint32_t btn) const { return !wantCaptureMouse_ && mouseButtons_[btn]; }

    // Raw (強制取得)
    bool IsKeyDownRaw(uint32_t vk) const { return keys_[vk]; }
    bool IsMouseButtonDownRaw(uint32_t btn) const { return mouseButtons_[btn]; }

    const Math::Vector2& GetMousePosition() const { return mousePosition_; }
    const Math::Vector2& GetMouseDelta() const { return mouseDelta_; }
    float GetMouseWheel() const { return mouseWheel_; }

    void SetMouseWheel(float wheel) { nextMouseWheel_ = wheel; }

private:
    std::array<bool, 256> keys_{};
    std::array<bool, 5> mouseButtons_{};
    Math::Vector2 mousePosition_{};
    Math::Vector2 mouseDelta_{};
    float mouseWheel_ = 0.0f;
    float nextMouseWheel_ = 0.0f;

    bool wantCaptureKeyboard_ = false;
    bool wantCaptureMouse_ = false;
};

} // namespace Engine::Core
