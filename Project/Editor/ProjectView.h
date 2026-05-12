#pragma once
#include <filesystem>
#include <vector>

namespace Engine::Editor {

class ProjectView {
public:
    ProjectView();
    void Render();

private:
    std::filesystem::path currentPath_;
};

} // namespace Engine::Editor
