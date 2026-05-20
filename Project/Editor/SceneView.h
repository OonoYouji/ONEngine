#pragma once

namespace Engine::Editor {

class SceneView {
public:
    void Render(const char* title = "Scene View", bool* p_open = nullptr);
};

} // namespace Engine::Editor
