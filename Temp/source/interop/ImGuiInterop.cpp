#include "ImGuiInterop.h"
#include <iostream>

// プロトタイプ段階ではコンソールに情報を出すだけの Mock 実装
// 本来は imgui.h をインクルードして実関数を呼び出す

EXPORT bool igBegin(const char* name, bool* p_open) {
    // std::cout << "[ImGui] Begin: " << name << std::endl;
    return true; 
}

EXPORT void igEnd() {
    // std::cout << "[ImGui] End" << std::endl;
}

EXPORT void igText(const char* fmt) {
    std::cout << "[ImGui] Text: " << fmt << std::endl;
}

EXPORT bool igButton(const char* label) {
    // std::cout << "[ImGui] Button: " << label << std::endl;
    return false;
}

EXPORT void igCheckbox(const char* label, bool* v) {
    // std::cout << "[ImGui] Checkbox: " << label << " = " << (*v ? "true" : "false") << std::endl;
}

EXPORT void igSliderFloat(const char* label, float* v, float v_min, float v_max) {
    // std::cout << "[ImGui] Slider: " << label << " = " << *v << std::endl;
}
