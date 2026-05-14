#include "ProjectView.h"
#include "imgui.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Common/Console.h"
#include <algorithm>

namespace Engine::Editor {

ProjectView::ProjectView() {
    assetsRoot_ = std::filesystem::current_path() / "Assets";
    if (!std::filesystem::exists(assetsRoot_)) {
        assetsRoot_ = std::filesystem::current_path();
    }
    currentPath_ = assetsRoot_;
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
    if (!std::filesystem::exists(currentPath_)) return;

    try {
        lastWriteTime_ = std::filesystem::last_write_time(currentPath_);

        for (auto& p : std::filesystem::directory_iterator(currentPath_)) {
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
        currentPath_ = pendingPath_;
        pendingPath_ = "";
        needsRefresh_ = true;
    }

    if (needsRefresh_) {
        RefreshCache();
    }

    if (std::filesystem::exists(currentPath_)) {
        try {
            auto currentTime = std::filesystem::last_write_time(currentPath_);
            if (currentTime != lastWriteTime_) needsRefresh_ = true;
        } catch (...) {}
    }

    ImGui::Begin("Project");

    if (ImGui::IsWindowHovered() && ImGui::GetIO().KeyCtrl) {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0) {
            thumbnailSize_ += wheel * 10.0f;
            thumbnailSize_ = (std::max)(32.0f, (std::min)(thumbnailSize_, 256.0f));
        }
    }

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
    if (currentPath_ == path) flags |= ImGuiTreeNodeFlags_Selected;

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
    
    if (ImGui::IsItemClicked()) {
        pendingPath_ = path;
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

        // 全体サイズを計算 (アイコン + 名前 2行分)
        ImVec2 itemSize(thumbnailSize_, thumbnailSize_ + ImGui::GetTextLineHeightWithSpacing() * 2.5f);
        ImVec2 startCursorPos = ImGui::GetCursorPos();

        // 1. コンテンツの描画 (Image, Text は ID を持たない)
        ImGui::BeginGroup();
        int32_t iconIndex = entry.iconIndex;
        if (iconIndex >= 0) {
            ImGui::Image((ImTextureID)srvHeap->GetGPUHandle(iconIndex).ptr, ImVec2(thumbnailSize_, thumbnailSize_));
        } else {
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImU32 color = entry.isDirectory ? IM_COL32(200, 200, 50, 255) : IM_COL32(100, 100, 100, 255);
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + thumbnailSize_, p.y + thumbnailSize_), color);
            ImGui::Dummy(ImVec2(thumbnailSize_, thumbnailSize_));
        }
        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + thumbnailSize_);
        ImGui::Text("%s", entry.name.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndGroup();

        // 2. 見えないボタンを重ねて ID とインタラクションを提供
        // これにより LastItemData.ID が有効になり BeginDragDropSource(0) が通るようになる
        ImGui::SetCursorPos(startCursorPos);
        ImGui::InvisibleButton("##hitarea", itemSize);

        // 3. ナビゲーション
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            if (entry.isDirectory) nextPath = entry.path;
        }

        // 4. ドラッグ＆ドロップ (LastItem が InvisibleButton なので IDAssertion が発生しない)
        if (!entry.isDirectory && ImGui::BeginDragDropSource(0)) {
            std::string rel = std::filesystem::relative(entry.path, std::filesystem::current_path()).string();
            std::replace(rel.begin(), rel.end(), '\\', '/');
            ImGui::SetDragDropPayload("DND_ASSET_PATH", rel.c_str(), rel.length() + 1);
            ImGui::Text("%s", entry.name.c_str());
            ImGui::EndDragDropSource();
        }

        // 5. グリッド配置の計算
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
}

} // namespace Engine::Editor
