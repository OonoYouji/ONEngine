#include "ProjectView.h"
#include "imgui.h"
#include <algorithm>

namespace Engine::Editor {

ProjectView::ProjectView() {
    // 実行ファイルの場所から相対的に Assets フォルダを探す
    currentPath_ = std::filesystem::current_path() / "Assets";
    if (!std::filesystem::exists(currentPath_)) {
        // デバッグ環境等で階層が異なる場合のフォールバック
        currentPath_ = std::filesystem::current_path();
    }
}

void ProjectView::Render() {
    ImGui::Begin("Project");

    // ナビゲーションバー
    std::filesystem::path assetsRoot = std::filesystem::current_path() / "Assets";

    if (ImGui::Button("Root")) {
        currentPath_ = assetsRoot;
    }
    ImGui::SameLine();

    if (currentPath_ != assetsRoot && currentPath_.has_parent_path()) {
        if (ImGui::Button("..")) {
            currentPath_ = currentPath_.parent_path();
        }
        ImGui::SameLine();
    }

    ImGui::Text("Path: %s", currentPath_.string().c_str());
    ImGui::Separator();

    // ファイル・ディレクトリ一覧
    if (std::filesystem::exists(currentPath_)) {
        for (auto& p : std::filesystem::directory_iterator(currentPath_)) {
            auto path = p.path();
            auto fileName = path.filename().string();

            // .meta ファイルや隠しファイルは表示しない
            if (fileName.find(".meta") != std::string::npos || fileName[0] == '.') {
                continue;
            }

            if (p.is_directory()) {
                if (ImGui::Selectable(("[DIR] " + fileName).c_str())) {
                    currentPath_ /= path.filename();
                    break; // イテレータが無効になる可能性があるため一旦抜ける
                }
            } else {
                std::string label = "[FILE] " + fileName;
                ImGui::Selectable(label.c_str());

                if (ImGui::BeginDragDropSource()) {
                    std::string fullPath = path.string();

                    // エンジン側のロード関数が相対パス（Assets/からのパス）を期待している場合を考慮
                    std::string relativePath = std::filesystem::relative(path, std::filesystem::current_path()).string();
                    std::replace(relativePath.begin(), relativePath.end(), '\\', '/');

                    ImGui::SetDragDropPayload("DND_ASSET_PATH", relativePath.c_str(), relativePath.length() + 1);
                    ImGui::Text("Dragging %s", fileName.c_str());
                    ImGui::EndDragDropSource();
                }
            }
        }
    } else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Directory not found!");
    }

    ImGui::End();
}

} // namespace Engine::Editor

