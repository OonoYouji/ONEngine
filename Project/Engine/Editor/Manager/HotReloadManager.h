#pragma once

#include <string>
#include <vector>
#include <mutex>

namespace Editor {

class HotReloadManager {
public:
    static HotReloadManager& GetInstance() {
        static HotReloadManager instance;
        return instance;
    }

    void RequestAssetReload(const std::string& path) {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingAssetReloads_.push_back(path);
    }

    void RequestScriptHotReload() {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingScriptHotReload_ = true;
    }

    struct Requests {
        std::vector<std::string> assetPaths;
        bool scriptHotReload;
    };

    Requests ConsumeRequests() {
        std::lock_guard<std::mutex> lock(mutex_);
        Requests reqs = { std::move(pendingAssetReloads_), pendingScriptHotReload_ };
        pendingAssetReloads_.clear();
        pendingScriptHotReload_ = false;
        return reqs;
    }

private:
    HotReloadManager() = default;
    std::mutex mutex_;
    std::vector<std::string> pendingAssetReloads_;
    bool pendingScriptHotReload_ = false;
};

} /// Editor
