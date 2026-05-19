#include "Engine/Core/RawInputService.h"
#include "imgui.h"

namespace Engine::Core {

void RawInputService::Update(HWND hwnd) {
#ifdef ENGINE_EDITOR
    ImGuiIO& io = ImGui::GetIO();
    wantCaptureKeyboard_ = io.WantCaptureKeyboard;
    wantCaptureMouse_ = io.WantCaptureMouse;
#endif

    // キーボード (Raw)
    for (uint32_t i = 0; i < 256; ++i) {
        keys_[i] = (GetKeyState(i) & 0x8000) != 0;
    }

    // マウス (Raw)
    mouseButtons_[0] = (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
    mouseButtons_[1] = (GetKeyState(VK_RBUTTON) & 0x8000) != 0;
    mouseButtons_[2] = (GetKeyState(VK_MBUTTON) & 0x8000) != 0;
    mouseButtons_[3] = (GetKeyState(VK_XBUTTON1) & 0x8000) != 0;
    mouseButtons_[4] = (GetKeyState(VK_XBUTTON2) & 0x8000) != 0;

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hwnd, &pt);
    
    Math::Vector2 newPos = { static_cast<float>(pt.x), static_cast<float>(pt.y) };
    mouseDelta_ = newPos - mousePosition_;
    mousePosition_ = newPos;

    mouseWheel_ = nextMouseWheel_;
    nextMouseWheel_ = 0.0f;
}

} // namespace Engine::Core
