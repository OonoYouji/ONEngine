#pragma once
#include <filesystem>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <fstream>
#include <Windows.h>
#include <shellapi.h>
#include "imgui.h"

namespace Engine::Editor {

struct ProjectEntry {
    std::filesystem::path path;
    std::string name;
    bool isDirectory;
    int32_t iconIndex;
};

struct ProjectTab {
    std::filesystem::path currentPath;
    std::string name;
    uint32_t id;
    
    // Navigation history
    std::vector<std::filesystem::path> backStack;
    std::vector<std::filesystem::path> forwardStack;
};

class ProjectView {
public:
    ProjectView();
    void Render(bool* p_open = nullptr);

private:
    void GoBack();
    void GoForward();
    void RecordHistory(const std::filesystem::path& newPath);
    std::filesystem::path assetsRoot_;
    
    // Multi-tab support
    std::vector<ProjectTab> tabs_;
    uint32_t nextTabId_ = 0;
    int activeTabIndex_ = 0;
    int tabToSelect_ = -1;
    
    std::filesystem::path pendingPath_;
    bool needsRefresh_ = false;
    
    std::filesystem::file_time_type lastWriteTime_;
    std::vector<ProjectEntry> cachedEntries_;
    
    float thumbnailSize_ = 64.0f;
    
    // アイコンのテクスチャインデックスを保持
    std::unordered_map<std::string, int32_t> iconMap_;
    
    // Clipboard for file operations
    std::vector<std::filesystem::path> clipboardPaths_;
    bool isCutOperation_ = false;
    
    // Rename state
    std::filesystem::path renameTargetPath_;
    char renameBuffer_[256];
    bool isRenaming_ = false;

    // Multi-Selection
    std::set<std::filesystem::path> selectedPaths_;
    std::filesystem::path lastSelectedPath_;
    
    // Box Selection
    bool isBoxSelecting_ = false;
    ImVec2 boxStartPos_;
    
    void LoadIcons();
    void RefreshCache();
    int32_t GetIconForPath(const std::filesystem::path& path);
    
    void RenderTabs();
    void RenderBreadcrumbs();
    void RenderTree(const std::filesystem::path& path);
    void RenderContent();
    void RenderContextMenu();
    
    void CreateNewFolder(const std::filesystem::path& parentPath);
    void CreateNewMaterial(const std::filesystem::path& parentPath);
    void DeletePaths(const std::set<std::filesystem::path>& paths);
    void DuplicatePaths(const std::set<std::filesystem::path>& paths);
    void RenamePath(const std::filesystem::path& oldPath, const std::string& newName);
    void PasteClipboard(const std::filesystem::path& destinationFolder);
    void AddTab(const std::filesystem::path& path);
};

} // namespace Engine::Editor
