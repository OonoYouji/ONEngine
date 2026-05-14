#include "ProjectView.h"
#include "imgui.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Common/Console.h"
#include <algorithm>
#include <cstring>

namespace Engine::Editor {

ProjectView::ProjectView() {
    assetsRoot_ = std::filesystem::current_path() / "Assets";
    if (!std::filesystem::exists(assetsRoot_)) {
        assetsRoot_ = std::filesystem::current_path();
    }
    
    // 初期タブの追加
    ProjectTab initialTab;
    initialTab.currentPath = assetsRoot_;
    initialTab.name = "Assets";
    tabs_.push_back(initialTab);
    activeTabIndex_ = 0;

    pendingPath_ = "";
    needsRefresh_ = false;
    
    LoadIcons();
    RefreshCache();
}

void ProjectView::LoadIcons() {
    auto& tm = Asset::TextureManager::GetInstance();
    
    std::vector<std::string> searchPaths = {
        "Packages/Textures/Editor/Icon/",
        "Project/Packages/Textures/Editor/Icon/",
        "../Packages/Textures/Editor/Icon/",
        "../../Project/Packages/Textures/Editor/Icon/"
    };

    std::string iconPath = "";
    for (const auto& p : searchPaths) {
        if (std::filesystem::exists(p)) {
            iconPath = p;
            break;
        }
    }

    auto LoadSafe = [&](const std::string& key, const std::string& fileName) {
        int32_t index = tm.LoadTexture(iconPath + fileName);
        iconMap_[key] = (index >= 0) ? index : -1;
    };

    LoadSafe("folder", "folder.png");
    LoadSafe(".cs", "csharp.png");
    LoadSafe(".mat", "material.png");
    LoadSafe(".prefab", "prefab.png");
    LoadSafe(".ttf", "font.png");
}

void ProjectView::RefreshCache() {
    cachedEntries_.clear();
    if (activeTabIndex_ >= tabs_.size()) return;
    const auto& currentPath = tabs_[activeTabIndex_].currentPath;
    if (!std::filesystem::exists(currentPath)) return;

    try {
        lastWriteTime_ = std::filesystem::last_write_time(currentPath);

        for (auto& p : std::filesystem::directory_iterator(currentPath)) {
            auto path = p.path();
            auto fileName = path.filename().string();
            if (fileName.find(".meta") != std::string::npos || fileName[0] == '.') continue;

            ProjectEntry entry;
            entry.path = path;
            entry.name = fileName;
            entry.isDirectory = p.is_directory();
            entry.iconIndex = GetIconForPath(path);
            cachedEntries_.push_back(entry);
        }

        std::sort(cachedEntries_.begin(), cachedEntries_.end(), [](const ProjectEntry& a, const ProjectEntry& b) {
            if (a.isDirectory != b.isDirectory) return a.isDirectory;
            return a.name < b.name;
        });
    } catch (...) {}
    needsRefresh_ = false;
}

int32_t ProjectView::GetIconForPath(const std::filesystem::path& path) {
    if (std::filesystem::is_directory(path)) {
        return iconMap_.count("folder") ? iconMap_["folder"] : -1;
    }

    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (iconMap_.count(ext)) return iconMap_[ext];

    if (ext == ".png" || ext == ".jpg" || ext == ".tga" || ext == ".dds" || ext == ".bmp") {
        std::string relativePath = std::filesystem::relative(path, std::filesystem::current_path()).string();
        std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
        return Asset::TextureManager::GetInstance().LoadTexture(relativePath);
    }

    return -1;
}

void ProjectView::Render() {
    if (!pendingPath_.empty()) {
        if (activeTabIndex_ >= 0 && activeTabIndex_ < (int)tabs_.size()) {
            tabs_[activeTabIndex_].currentPath = pendingPath_;
            tabs_[activeTabIndex_].name = pendingPath_.filename().string();
            if (tabs_[activeTabIndex_].name.empty()) tabs_[activeTabIndex_].name = "Assets";
        }
        pendingPath_ = "";
        needsRefresh_ = true;
    }

    if (needsRefresh_) {
        RefreshCache();
    }

    if (activeTabIndex_ >= 0 && activeTabIndex_ < (int)tabs_.size()) {
        const auto& currentPath = tabs_[activeTabIndex_].currentPath;
        if (std::filesystem::exists(currentPath)) {
            try {
                auto currentTime = std::filesystem::last_write_time(currentPath);
                if (currentTime != lastWriteTime_) needsRefresh_ = true;
            } catch (...) {}
        }
    }

    ImGui::Begin("Project");

    if (ImGui::IsWindowHovered() && ImGui::GetIO().KeyCtrl) {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0) {
            thumbnailSize_ += wheel * 10.0f;
            thumbnailSize_ = (std::max)(32.0f, (std::min)(thumbnailSize_, 256.0f));
        }
    }

    // タブのレンダリング
    RenderTabs();

    if (ImGui::BeginTable("ProjectSplit", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings)) {
        ImGui::TableSetupColumn("Tree", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        if (ImGui::BeginChild("TreePane")) {
            RenderTree(assetsRoot_);
        }
        ImGui::EndChild();

        ImGui::TableSetColumnIndex(1);
        if (ImGui::BeginChild("ContentPane")) {
            RenderBreadcrumbs();
            ImGui::Separator();
            RenderContent();
        }
        ImGui::EndChild();

        ImGui::EndTable();
    }

    ImGui::End();
}

void ProjectView::RenderTree(const std::filesystem::path& path) {
    std::string name = path.filename().string();
    if (name.empty()) name = "Assets";

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (activeTabIndex_ < (int)tabs_.size() && tabs_[activeTabIndex_].currentPath == path) flags |= ImGuiTreeNodeFlags_Selected;

    bool hasSubDir = false;
    try {
        if (std::filesystem::is_directory(path)) {
            for (auto& p : std::filesystem::directory_iterator(path)) {
                if (p.is_directory()) { hasSubDir = true; break; }
            }
        }
    } catch (...) {}
    
    if (!hasSubDir) flags |= ImGuiTreeNodeFlags_Leaf;

    ImGui::PushID(path.string().c_str());
    bool opened = ImGui::TreeNodeEx("##node", flags, "%s", name.c_str());
    
    if (ImGui::IsItemClicked(0)) {
        if (ImGui::GetIO().KeyCtrl) {
            AddTab(path);
        } else {
            pendingPath_ = path;
        }
    }

    if (opened) {
        if (hasSubDir) {
            try {
                for (auto& p : std::filesystem::directory_iterator(path)) {
                    if (p.is_directory()) RenderTree(p.path());
                }
            } catch (...) {}
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void ProjectView::RenderTabs() {
    if (ImGui::BeginTabBar("ProjectTabs", ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable)) {
        for (int i = 0; i < (int)tabs_.size(); i++) {
            bool open = true;
            ImGuiTabItemFlags flags = (i == tabToSelect_) ? ImGuiTabItemFlags_SetSelected : 0;
            
            bool visible = ImGui::BeginTabItem(tabs_[i].name.c_str(), &open, flags);
            if (i == tabToSelect_) tabToSelect_ = -1;

            if (visible) {
                if (activeTabIndex_ != i) {
                    activeTabIndex_ = i;
                    needsRefresh_ = true;
                }
                ImGui::EndTabItem();
            }

            if (!open) {
                tabs_.erase(tabs_.begin() + i);
                if (activeTabIndex_ >= (int)tabs_.size()) activeTabIndex_ = (int)tabs_.size() - 1;
                if (activeTabIndex_ < 0) {
                    AddTab(assetsRoot_);
                }
                i--;
                needsRefresh_ = true;
            }
        }

        if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoReorder)) {
            AddTab(assetsRoot_);
        }
        ImGui::EndTabBar();
    }
}
// ... (RenderBreadcrumbs remains mostly same, just ensuring it uses pendingPath_ correctly)
void ProjectView::RenderBreadcrumbs() {
    if (activeTabIndex_ < 0 || activeTabIndex_ >= (int)tabs_.size()) return;
    const auto& currentPath = tabs_[activeTabIndex_].currentPath;
// ... (omitted lines for brevity in instruction, but I'll provide full replacement block below)

    std::filesystem::path rel = std::filesystem::relative(currentPath, assetsRoot_.parent_path());
    
    std::vector<std::filesystem::path> components;
    for (auto& p : rel) components.push_back(p);

    std::filesystem::path walk = assetsRoot_.parent_path();
    for (size_t i = 0; i < components.size(); i++) {
        walk /= components[i];
        
        std::string label = components[i].string();
        if (label == "Assets" && i == 0) label = "Assets"; // Root name

        if (i > 0) {
            ImGui::SameLine();
            ImGui::TextDisabled(">");
            ImGui::SameLine();
        }

        if (ImGui::Button(label.c_str())) {
            pendingPath_ = walk;
        }
    }
}

void ProjectView::RenderContent() {
    auto& style = ImGui::GetStyle();
    float padding = 16.0f;
    float cellSize = thumbnailSize_ + padding;
    float panelWidth = ImGui::GetContentRegionAvail().x;
    
    auto& graphics = Graphics::GraphicsEngine::GetInstance();
    auto* srvHeap = graphics.GetSRVHeap();
    std::filesystem::path nextPath = "";

    float x_start = ImGui::GetCursorPosX();

    for (size_t i = 0; i < cachedEntries_.size(); ++i) {
        const auto& entry = cachedEntries_[i];
        ImGui::PushID((int)i);

        ImVec2 itemSize(thumbnailSize_, thumbnailSize_ + ImGui::GetTextLineHeightWithSpacing() * 2.5f);
        ImVec2 startCursorPos = ImGui::GetCursorPos();

        // 1. コンテンツの描画
        ImGui::BeginGroup();
        
        bool selected = (selectedPath_ == entry.path);
        if (selected) {
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + thumbnailSize_, p.y + itemSize.y), ImGui::GetColorU32(ImGuiCol_HeaderActive), 4.0f);
        }

        int32_t iconIndex = entry.iconIndex;
        if (iconIndex >= 0) {
            ImGui::Image((ImTextureID)srvHeap->GetGPUHandle(iconIndex).ptr, ImVec2(thumbnailSize_, thumbnailSize_));
        } else {
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImU32 color = entry.isDirectory ? IM_COL32(200, 200, 50, 255) : IM_COL32(100, 100, 100, 255);
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + thumbnailSize_, p.y + thumbnailSize_), color, 4.0f);
            ImGui::Dummy(ImVec2(thumbnailSize_, thumbnailSize_));
        }
        
        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + thumbnailSize_);
        if (isRenaming_ && renameTargetPath_ == entry.path) {
            ImGui::SetNextItemWidth(thumbnailSize_);
            if (ImGui::InputText("##rename", renameBuffer_, sizeof(renameBuffer_), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
                RenamePath(entry.path, renameBuffer_);
                isRenaming_ = false;
            }
            if (!ImGui::IsItemActive() && ImGui::IsMouseClicked(0)) isRenaming_ = false;
        } else {
            ImGui::Text("%s", entry.name.c_str());
        }
        ImGui::PopTextWrapPos();
        ImGui::EndGroup();

        ImGui::SetCursorPos(startCursorPos);
        ImGui::InvisibleButton("##hitarea", itemSize);

        if (ImGui::IsItemClicked(0)) {
            selectedPath_ = entry.path;
        }
        
        if (ImGui::IsItemClicked(1)) {
            selectedPath_ = entry.path;
            ImGui::OpenPopup("ItemContextMenu");
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            if (entry.isDirectory) {
                if (ImGui::GetIO().KeyCtrl) {
                    AddTab(entry.path);
                } else {
                    nextPath = entry.path;
                }
            }
        }

        if (!entry.isDirectory && ImGui::BeginDragDropSource(0)) {
            std::string rel = std::filesystem::relative(entry.path, std::filesystem::current_path()).string();
            std::replace(rel.begin(), rel.end(), '\\', '/');
            ImGui::SetDragDropPayload("DND_ASSET_PATH", rel.c_str(), rel.length() + 1);
            ImGui::Text("%s", entry.name.c_str());
            ImGui::EndDragDropSource();
        }

        float last_item_x2 = ImGui::GetItemRectMax().x;
        float next_item_x2 = last_item_x2 + style.ItemSpacing.x + cellSize;
        if (i + 1 < cachedEntries_.size() && next_item_x2 < (x_start + panelWidth)) {
            ImGui::SameLine();
        }

        ImGui::PopID();
    }

    if (!nextPath.empty()) {
        pendingPath_ = nextPath;
    }

    // 背景右クリックでのコンテキストメニュー
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::IsMouseClicked(1) && !ImGui::IsAnyItemHovered()) {
        ImGui::OpenPopup("FolderContextMenu");
    }

    RenderContextMenu();
}

void ProjectView::RenderContextMenu() {
    if (ImGui::BeginPopup("ItemContextMenu")) {
        if (ImGui::MenuItem("Rename")) {
            isRenaming_ = true;
            renameTargetPath_ = selectedPath_;
            strncpy(renameBuffer_, selectedPath_.filename().string().c_str(), sizeof(renameBuffer_));
        }
        if (ImGui::MenuItem("Delete")) {
            DeletePath(selectedPath_);
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Cut")) {
            clipboardPath_ = selectedPath_;
            isCutOperation_ = true;
        }
        if (ImGui::MenuItem("Copy")) {
            clipboardPath_ = selectedPath_;
            isCutOperation_ = false;
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("FolderContextMenu")) {
        if (ImGui::MenuItem("New Folder")) {
            CreateNewFolder(tabs_[activeTabIndex_].currentPath);
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Paste", nullptr, false, !clipboardPath_.empty())) {
            PasteClipboard(tabs_[activeTabIndex_].currentPath);
        }
        ImGui::EndPopup();
    }
}

void ProjectView::CreateNewFolder(const std::filesystem::path& parentPath) {
    std::filesystem::path newPath = parentPath / "New Folder";
    int i = 1;
    while (std::filesystem::exists(newPath)) {
        newPath = parentPath / ("New Folder (" + std::to_string(i++) + ")");
    }
    std::filesystem::create_directory(newPath);
    needsRefresh_ = true;
}

void ProjectView::DeletePath(const std::filesystem::path& path) {
    try {
        if (std::filesystem::exists(path)) {
            std::filesystem::remove_all(path);
            needsRefresh_ = true;
        }
    } catch (...) {}
}

void ProjectView::RenamePath(const std::filesystem::path& oldPath, const std::string& newName) {
    try {
        std::filesystem::path newPath = oldPath.parent_path() / newName;
        if (!std::filesystem::exists(newPath)) {
            std::filesystem::rename(oldPath, newPath);
            needsRefresh_ = true;
        }
    } catch (...) {}
}

void ProjectView::PasteClipboard(const std::filesystem::path& destinationFolder) {
    if (clipboardPath_.empty()) return;

    try {
        std::filesystem::path dest = destinationFolder / clipboardPath_.filename();
        if (isCutOperation_) {
            std::filesystem::rename(clipboardPath_, dest);
            clipboardPath_ = "";
        } else {
            if (std::filesystem::is_directory(clipboardPath_)) {
                std::filesystem::copy(clipboardPath_, dest, std::filesystem::copy_options::recursive);
            } else {
                std::filesystem::copy(clipboardPath_, dest);
            }
        }
        needsRefresh_ = true;
    } catch (...) {}
}

void ProjectView::AddTab(const std::filesystem::path& path) {
    ProjectTab newTab;
    newTab.currentPath = path;
    newTab.name = path.filename().string();
    if (newTab.name.empty()) newTab.name = "Assets";
    tabs_.push_back(newTab);
    tabToSelect_ = (int)tabs_.size() - 1;
    needsRefresh_ = true;
}

} // namespace Engine::Editor
