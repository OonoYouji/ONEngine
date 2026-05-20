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
            color = IM_COL32(255, 255, 0, 255); // フォーカス時は黄色
            thickness = 2.0f;
        }
        else if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::GetDragDropPayload()) {
            color = IM_COL32(230, 133, 93, 255); // ドラッグ中
            thickness = 2.0f;
        }

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 min = windowPos;
        ImVec2 max = ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y);
        
        // 線の太さの半分だけ内側に寄せる（ウィンドウの外側に食み出さないようにする）
        float halfThickness = thickness * 0.5f;
        min.x += halfThickness; min.y += halfThickness;
        max.x -= halfThickness; max.y -= halfThickness;

        // WindowDrawList を使用（他のウィンドウやポップアップより背面に描画される）
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        
        // スクロールバーなどはウィンドウの End() 時に描画されることが多いため、
        // コンテンツ領域のクリッピングを一時的に無効化（ウィンドウ全体を許容）して描画する
        drawList->PushClipRect(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y), false);
        drawList->AddRect(min, max, color, 0.0f, 0, thickness);
        drawList->PopClipRect();
    }

    /// @brief ファイルを開くダイアログを表示
    /// @param filter フィルタ (例: "Scene Files (*.scene)\0*.scene\0All Files (*.*)\0*.*\0")
    /// @param initialDir 初期ディレクトリ (絶対パス推奨)
    /// @return 選択されたパス。キャンセル時はstd::nullopt
    static std::optional<std::string> OpenFileDialog(const char* filter, const char* initialDir = nullptr) {
        OPENFILENAMEA ofn;
        char szFile[260] = { 0 };
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = GetActiveWindow();
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.lpstrInitialDir = initialDir;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn)) {
            return std::string(szFile);
        }
        return std::nullopt;
    }

    /// @brief ファイルを保存するダイアログを表示
    /// @param filter フィルタ
    /// @param initialDir 初期ディレクトリ (絶対パス推奨)
    /// @return 選択されたパス。キャンセル時はstd::nullopt
    static std::optional<std::string> SaveFileDialog(const char* filter, const char* initialDir = nullptr) {
        OPENFILENAMEA ofn;
        char szFile[260] = { 0 };
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = GetActiveWindow();
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.lpstrInitialDir = initialDir;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        if (GetSaveFileNameA(&ofn)) {
            return std::string(szFile);
        }
        return std::nullopt;
    }
};

} // namespace Engine::Editor
