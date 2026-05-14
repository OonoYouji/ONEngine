#pragma once

#include <string>
#include <optional>
#include <windows.h>
#include <commdlg.h>

namespace Engine::Editor {

class EditorUtils {
public:
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
