#include "ProjectView.h"
#include "imgui.h"

namespace Engine::Editor {

ProjectView::ProjectView() {
    currentPath_ = std::filesystem::current_path() / "Assets";
}

void ProjectView::Render() {
    ImGui::Begin("Project");

    if (currentPath_ != std::filesystem::current_path() / "Assets") {
        if (ImGui::Button("<- Back")) {
            currentPath_ = currentPath_.parent_path();
        }
    }

    ImGui::Separator();

    for (auto& p : std::filesystem::directory_iterator(currentPath_)) {
        auto path = p.path();
        auto fileName = path.filename().string();

        if (p.is_directory()) {
            if (ImGui::Selectable(("[Dir] " + fileName).c_str())) {
                currentPath_ /= path.filename();
            }
        } else {
            ImGui::Selectable(("[File] " + fileName).c_str());
            if (ImGui::BeginDragDropSource()) {
                std::string fullPath = path.string();
                // "DND_ASSET_PATH" という名前でパスをペイロードとして渡す
                ImGui::SetDragDropPayload("DND_ASSET_PATH", fullPath.c_str(), fullPath.length() + 1);
                ImGui::Text("Dragging %s", fileName.c_str());
                ImGui::EndDragDropSource();
            }
        }
    }

    ImGui::End();
}

} // namespace Engine::Editor
