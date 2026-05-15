#include "ProjectView.h"
#include "imgui.h"
#include "EditorContext.h"
#include "EditorUtils.h"
#include "Engine/Core/Application.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Scene/SceneLoader.h"
#include "Engine/Common/Console.h"
#include "Externals/nlohmann/json.hpp"
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
    initialTab.id = nextTabId_++;
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

void ProjectView::Render(bool* p_open) {
    if (p_open && !*p_open) return;
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

    EditorUtils::DrawActiveViewOutline();
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
        int tabToDelete = -1;
        for (int i = 0; i < (int)tabs_.size(); i++) {
            bool open = true;
            bool* p_open = (tabs_.size() > 1) ? &open : nullptr;
            ImGuiTabItemFlags flags = (i == tabToSelect_) ? ImGuiTabItemFlags_SetSelected : 0;
            
            // タブ名にIDを隠しIDとして付与して一意にする
            std::string label = tabs_[i].name + "###tab_" + std::to_string(tabs_[i].id);
            bool visible = ImGui::BeginTabItem(label.c_str(), p_open, flags);

            if (visible) {
                if (activeTabIndex_ != i) {
                    activeTabIndex_ = i;
                    needsRefresh_ = true;
                }
                ImGui::EndTabItem();
            }

            if (!open) {
                tabToDelete = i;
            }
        }

        if (tabToDelete != -1) {
            tabs_.erase(tabs_.begin() + tabToDelete);
            if (activeTabIndex_ >= (int)tabs_.size()) activeTabIndex_ = (int)tabs_.size() - 1;
            if (activeTabIndex_ < 0) {
                AddTab(assetsRoot_);
            }
            needsRefresh_ = true;
        }

        tabToSelect_ = -1;

        if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoReorder)) {
            AddTab(assetsRoot_);
        }
        ImGui::EndTabBar();
    }
}

void ProjectView::RenderBreadcrumbs() {
    if (activeTabIndex_ < 0 || activeTabIndex_ >= (int)tabs_.size()) return;
    const auto& currentPath = tabs_[activeTabIndex_].currentPath;

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
    bool openItemMenu = false;
    
    ImVec2 contentRegionStart = ImGui::GetCursorScreenPos();
    ImGuiIO& io = ImGui::GetIO();

    // アイテムごとの矩形情報を保持する一時構造体
    struct ItemRect {
        std::filesystem::path path;
        ImVec2 min;
        ImVec2 max;
    };
    std::vector<ItemRect> itemRects;

    for (size_t i = 0; i < cachedEntries_.size(); ++i) {
        const auto& entry = cachedEntries_[i];
        ImGui::PushID((int)i);

        ImVec2 itemSize(thumbnailSize_, thumbnailSize_ + ImGui::GetTextLineHeightWithSpacing() * 2.5f);
        ImVec2 startCursorPos = ImGui::GetCursorPos();
        ImVec2 screenPos = ImGui::GetCursorScreenPos();

        itemRects.push_back({ entry.path, screenPos, ImVec2(screenPos.x + itemSize.x, screenPos.y + itemSize.y) });

        // 1. コンテンツの描画
        ImGui::BeginGroup();
        
        bool selected = (selectedPaths_.count(entry.path) > 0);
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

        if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1)) {
            bool ctrl = io.KeyCtrl;
            bool shift = io.KeyShift;

            if (shift && !lastSelectedPath_.empty()) {
                // Range selection
                bool selecting = false;
                for (const auto& e : cachedEntries_) {
                    if (e.path == entry.path || e.path == lastSelectedPath_) {
                        selectedPaths_.insert(e.path);
                        if (entry.path == lastSelectedPath_) {
                            // If start and end are the same, just select it
                        } else {
                             if (selecting) { selecting = false; }
                             else { selecting = true; }
                        }
                    } else if (selecting) {
                        selectedPaths_.insert(e.path);
                    }
                }
            } else if (ctrl) {
                if (selectedPaths_.count(entry.path)) selectedPaths_.erase(entry.path);
                else selectedPaths_.insert(entry.path);
            } else {
                if (!selectedPaths_.count(entry.path)) {
                    selectedPaths_.clear();
                    selectedPaths_.insert(entry.path);
                }
            }
            lastSelectedPath_ = entry.path;
        }
        
        if (ImGui::IsItemClicked(1)) {
            openItemMenu = true;
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            if (entry.isDirectory) {
                if (io.KeyCtrl) {
                    AddTab(entry.path);
                } else {
                    nextPath = entry.path;
                }
            }
        }

        if (!entry.isDirectory && ImGui::BeginDragDropSource(0)) {
            // 複数選択している場合でも、現在ドラッグ開始したアイテムを単体パスとして優先的に設定する
            std::string entryRel = std::filesystem::relative(entry.path, std::filesystem::current_path()).string();
            std::replace(entryRel.begin(), entryRel.end(), '\\', '/');
            ImGui::SetDragDropPayload("DND_ASSET_PATH", entryRel.c_str(), entryRel.length() + 1);

            // 複数選択している場合は、選択中の全パスを送る（一括処理用）
            std::string payload = "";
            for (const auto& p : selectedPaths_) {
                std::string rel = std::filesystem::relative(p, std::filesystem::current_path()).string();
                std::replace(rel.begin(), rel.end(), '\\', '/');
                payload += rel + "|";
            }
            ImGui::SetDragDropPayload("DND_ASSET_PATHS", payload.c_str(), payload.length() + 1);
            
            ImGui::Text("%zu items", selectedPaths_.size());
            ImGui::EndDragDropSource();
        }

        float last_item_x2 = ImGui::GetItemRectMax().x;
        float next_item_x2 = last_item_x2 + style.ItemSpacing.x + cellSize;
        if (i + 1 < cachedEntries_.size() && next_item_x2 < (x_start + panelWidth)) {
            ImGui::SameLine();
        }

        ImGui::PopID();
    }

    // --- Box Selection Logic ---
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
        if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) {
            isBoxSelecting_ = true;
            boxStartPos_ = io.MousePos;
            if (!io.KeyCtrl) selectedPaths_.clear();
        }
    }

    if (isBoxSelecting_) {
        if (ImGui::IsMouseReleased(0)) {
            isBoxSelecting_ = false;
        } else {
            ImVec2 mousePos = io.MousePos;
            ImVec2 boxMin = ImVec2((std::min)(boxStartPos_.x, mousePos.x), (std::min)(boxStartPos_.y, mousePos.y));
            ImVec2 boxMax = ImVec2((std::max)(boxStartPos_.x, mousePos.x), (std::max)(boxStartPos_.y, mousePos.y));

            // Draw selection box
            ImGui::GetForegroundDrawList()->AddRect(boxMin, boxMax, IM_COL32(100, 150, 255, 255));
            ImGui::GetForegroundDrawList()->AddRectFilled(boxMin, boxMax, IM_COL32(100, 150, 255, 50));

            // Select items inside box
            if (!io.KeyCtrl) selectedPaths_.clear();
            for (const auto& rect : itemRects) {
                // Check intersection
                if (rect.max.x < boxMin.x || rect.min.x > boxMax.x ||
                    rect.max.y < boxMin.y || rect.min.y > boxMax.y) {
                    continue;
                }
                selectedPaths_.insert(rect.path);
            }
        }
    }

    if (openItemMenu) {
        ImGui::OpenPopup("ItemContextMenu");
    }

    if (!nextPath.empty()) {
        pendingPath_ = nextPath;
        selectedPaths_.clear();
    }

    // 背景右クリックでのコンテキストメニュー
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::IsMouseClicked(1) && !ImGui::IsAnyItemHovered()) {
        selectedPaths_.clear();
        ImGui::OpenPopup("FolderContextMenu");
    }

    RenderContextMenu();
}

void ProjectView::RenderContextMenu() {
    if (ImGui::BeginPopup("ItemContextMenu")) {
        bool singleSelection = (selectedPaths_.size() == 1);
        std::filesystem::path firstPath = singleSelection ? *selectedPaths_.begin() : "";

        if (singleSelection) {
            bool isDirectory = std::filesystem::is_directory(firstPath);
            if (ImGui::MenuItem("Open")) {
                if (isDirectory) {
                    pendingPath_ = firstPath;
                } else if (firstPath.extension() == ".scene") {
                    auto& reg = Engine::Core::Application::GetInstance().GetRegistry();
                    reg.Clear();
                    Engine::Editor::EditorContext::GetInstance().SetCurrentScenePath(firstPath.string());
                    Engine::Scene::SceneLoader::LoadScene(firstPath.string(), reg);
                }
            }
            ImGui::Separator();
        }

        if (singleSelection && ImGui::MenuItem("Rename")) {
            isRenaming_ = true;
            renameTargetPath_ = firstPath;
            strncpy(renameBuffer_, firstPath.filename().string().c_str(), sizeof(renameBuffer_));
        }

        if (ImGui::MenuItem("Duplicate")) {
            DuplicatePaths(selectedPaths_);
        }

        if (ImGui::MenuItem("Delete")) {
            DeletePaths(selectedPaths_);
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Cut")) {
            clipboardPaths_.clear();
            for (const auto& p : selectedPaths_) clipboardPaths_.push_back(p);
            isCutOperation_ = true;
        }
        if (ImGui::MenuItem("Copy")) {
            clipboardPaths_.clear();
            for (const auto& p : selectedPaths_) clipboardPaths_.push_back(p);
            isCutOperation_ = false;
        }

        ImGui::Separator();

        if (singleSelection && std::filesystem::is_directory(firstPath)) {
            if (ImGui::BeginMenu("Create")) {
                if (ImGui::MenuItem("Folder")) {
                    CreateNewFolder(firstPath);
                }
                if (ImGui::MenuItem("Material")) {
                    CreateNewMaterial(firstPath);
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
        }

        if (ImGui::MenuItem("Show in Explorer")) {
            for (const auto& p : selectedPaths_) {
                ShellExecuteA(NULL, "explore", std::filesystem::is_directory(p) ? p.string().c_str() : p.parent_path().string().c_str(), NULL, NULL, SW_SHOW);
                if (!ImGui::GetIO().KeyShift) break; // Shift押してなければ最初の一つだけ
            }
        }

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("FolderContextMenu")) {
        if (ImGui::BeginMenu("Create")) {
            if (ImGui::MenuItem("Folder")) {
                CreateNewFolder(tabs_[activeTabIndex_].currentPath);
            }
            if (ImGui::MenuItem("Material")) {
                CreateNewMaterial(tabs_[activeTabIndex_].currentPath);
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Paste", nullptr, false, !clipboardPaths_.empty())) {
            PasteClipboard(tabs_[activeTabIndex_].currentPath);
        }
        if (ImGui::MenuItem("Show in Explorer")) {
            ShellExecuteA(NULL, "open", tabs_[activeTabIndex_].currentPath.string().c_str(), NULL, NULL, SW_SHOW);
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

void ProjectView::CreateNewMaterial(const std::filesystem::path& parentPath) {
    std::filesystem::path newPath = parentPath / "New Material.mat";
    int i = 1;
    while (std::filesystem::exists(newPath)) {
        newPath = parentPath / ("New Material (" + std::to_string(i++) + ").mat");
    }

    nlohmann::json data;
    data["name"] = newPath.stem().string();
    data["pipeline"] = "Assets/Pipelines/CelShader.json";
    data["texture"] = "";
    data["parameters"]["baseColor"] = { 1.0f, 1.0f, 1.0f, 1.0f };

    std::ofstream file(newPath);
    if (file.is_open()) {
        file << data.dump(4);
    }
    
    needsRefresh_ = true;
}

void ProjectView::DeletePaths(const std::set<std::filesystem::path>& paths) {
    for (const auto& path : paths) {
        try {
            if (std::filesystem::exists(path)) {
                std::filesystem::remove_all(path);
                
                // .meta も削除
                std::filesystem::path metaPath = path.string() + ".meta";
                if (std::filesystem::exists(metaPath)) {
                    std::filesystem::remove_all(metaPath);
                }
            }
        } catch (...) {}
    }
    selectedPaths_.clear();
    needsRefresh_ = true;
}

void ProjectView::DuplicatePaths(const std::set<std::filesystem::path>& paths) {
    for (const auto& path : paths) {
        try {
            std::filesystem::path parent = path.parent_path();
            std::string stem = path.stem().string();
            std::string ext = path.extension().string();
            
            std::filesystem::path newPath = parent / (stem + " - Copy" + ext);
            int i = 1;
            while (std::filesystem::exists(newPath)) {
                newPath = parent / (stem + " - Copy (" + std::to_string(i++) + ")" + ext);
            }

            if (std::filesystem::is_directory(path)) {
                std::filesystem::copy(path, newPath, std::filesystem::copy_options::recursive);
            } else {
                std::filesystem::copy(path, newPath);
            }
            
            // .meta もコピー
            std::filesystem::path oldMeta = path.string() + ".meta";
            std::filesystem::path newMeta = newPath.string() + ".meta";
            if (std::filesystem::exists(oldMeta)) {
                std::filesystem::copy(oldMeta, newMeta);
            }
        } catch (...) {}
    }
    needsRefresh_ = true;
}

void ProjectView::RenamePath(const std::filesystem::path& oldPath, const std::string& newName) {
    try {
        std::filesystem::path newPath = oldPath.parent_path() / newName;
        if (!std::filesystem::exists(newPath)) {
            std::filesystem::rename(oldPath, newPath);

            // .meta もリネーム
            std::filesystem::path oldMetaPath = oldPath.string() + ".meta";
            std::filesystem::path newMetaPath = newPath.string() + ".meta";
            if (std::filesystem::exists(oldMetaPath)) {
                std::filesystem::rename(oldMetaPath, newMetaPath);
            }

            needsRefresh_ = true;
        }
    } catch (...) {}
}

void ProjectView::PasteClipboard(const std::filesystem::path& destinationFolder) {
    if (clipboardPaths_.empty()) return;

    for (const auto& src : clipboardPaths_) {
        try {
            std::filesystem::path dest = destinationFolder / src.filename();
            std::filesystem::path oldMeta = src.string() + ".meta";
            std::filesystem::path newMeta = dest.string() + ".meta";

            if (isCutOperation_) {
                std::filesystem::rename(src, dest);
                if (std::filesystem::exists(oldMeta)) std::filesystem::rename(oldMeta, newMeta);
            } else {
                if (std::filesystem::is_directory(src)) {
                    std::filesystem::copy(src, dest, std::filesystem::copy_options::recursive);
                } else {
                    std::filesystem::copy(src, dest);
                    if (std::filesystem::exists(oldMeta)) std::filesystem::copy(oldMeta, newMeta);
                }
            }
        } catch (...) {}
    }
    if (isCutOperation_) clipboardPaths_.clear();
    needsRefresh_ = true;
}

void ProjectView::AddTab(const std::filesystem::path& path) {
    ProjectTab newTab;
    newTab.currentPath = path;
    newTab.name = path.filename().string();
    if (newTab.name.empty()) newTab.name = "Assets";
    newTab.id = nextTabId_++;
    tabs_.push_back(newTab);
    tabToSelect_ = (int)tabs_.size() - 1;
    needsRefresh_ = true;
}

} // namespace Engine::Editor
