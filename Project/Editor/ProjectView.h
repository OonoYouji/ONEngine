#pragma once
#include <filesystem>
#include <vector>
#include <string>
#include <unordered_map>

namespace Engine::Editor {

struct ProjectEntry {
    std::filesystem::path path;
    std::string name;
    bool isDirectory;
    int32_t iconIndex;
};

class ProjectView {
public:
    ProjectView();
    void Render();

private:
    std::filesystem::path assetsRoot_;
    std::filesystem::path currentPath_;
    std::filesystem::path pendingPath_;
    bool needsRefresh_ = false;
    
    std::filesystem::file_time_type lastWriteTime_;
    std::vector<ProjectEntry> cachedEntries_;
    
    float thumbnailSize_ = 64.0f;
    
    // アイコンのテクスチャインデックスを保持
    std::unordered_map<std::string, int32_t> iconMap_;
    
    void LoadIcons();
    void RefreshCache();
    int32_t GetIconForPath(const std::filesystem::path& path);
    
    void RenderTree(const std::filesystem::path& path);
    void RenderContent();
};

} // namespace Engine::Editor
