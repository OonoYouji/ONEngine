#pragma once
#include "imgui.h"
#include <vector>
#include <string>

namespace Engine::Editor {

class ConsoleView {
public:
    void Render();

private:
    bool autoScroll_ = true;
    char filterBuffer_[256] = { 0 };
    
    // 表示フラグ
    bool showInfo_ = true;
    bool showWarning_ = true;
    bool showError_ = true;
};

} // namespace Engine::Editor
