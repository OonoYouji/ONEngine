#pragma once

#include <string>
#include <optional>
#include <windows.h>
#include <commdlg.h>
#include "imgui.h"

namespace Engine::Editor {

class EditorUtils {
public:
    static void DrawActiveViewOutline() {
        ImU32 color = IM_COL32(40, 40, 40, 255); // 通常時の境界線
        float thickness = 1.0f;

        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
            color = IM_COL32(71, 114, 179, 255); // フォーカス時は Blender Blue
            thickness = 2.0f;
        }
        else if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::GetDragDropPayload()) {
            color = IM_COL32(230, 133, 93, 255); // ドラッグ中のホバーターゲットは Blender Orange
            thickness = 2.0f;
        }

        ImVec2 min = ImGui::GetWindowPos();
        ImVec2 max = ImVec2(min.x + ImGui::GetWindowSize().x, min.y + ImGui::GetWindowSize().y);
        
        // ウィンドウ全体の枠線を描画（ForegroundDrawListを使用することで、常に最前面に表示）
        ImGui::GetForegroundDrawList()->AddRect(min, max, color, 0.0f, 0, thickness);
    }

    /// @brief ファイルを開くダイアログを表示
    /// @param filter フィルタ (例: "Scene Files (*.scene)\0*.scene\0All Files (*.*)\0*.*\0")
    /// @return 選択されたパス。キャンセル時はstd::nullopt
    static std::optional<std::string> OpenFileDialog(const char* filter) {
        OPENFILENAMEA ofn;
        char szFile[260] = { 0 };
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = GetActiveWindow();
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn)) {
            return std::string(szFile);
        }
        return std::nullopt;
    }

    /// @brief ファイルを保存するダイアログを表示
    /// @param filter フィルタ
    /// @return 選択されたパス。キャンセル時はstd::nullopt
    static std::optional<std::string> SaveFileDialog(const char* filter) {
        OPENFILENAMEA ofn;
        char szFile[260] = { 0 };
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = GetActiveWindow();
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        if (GetSaveFileNameA(&ofn)) {
            return std::string(szFile);
        }
        return std::nullopt;
    }
};

} // namespace Engine::Editor
