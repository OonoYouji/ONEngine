#include "AssetDatabase.h"
#include "Externals/nlohmann/json.hpp"
#include "Engine/Common/Console.h"
#include <fstream>
#include <algorithm>
#include <random>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace Engine::Asset {

AssetDatabase* AssetDatabase::instance_ = nullptr;

namespace {
    bool IsAssetFile(const fs::path& path) {
        std::string ext = path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext == ".png" || ext == ".jpg" || ext == ".obj" || ext == ".fbx" || ext == ".mat" || ext == ".scene" || ext == ".ttf";
    }
}

void AssetDatabase::Scan(const std::string& directory) {
	if(!fs::exists(directory)) return;

	Engine::Console::Log(std::format("Scanning assets in: {}", directory));

	fs::path projectRoot = fs::current_path();

	for(const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_directory()) continue;
        
        fs::path path = entry.path();
        if (path.extension() == ".meta") continue;
        if (!IsAssetFile(path)) continue;

        fs::path metaPath = path.string() + ".meta";
        uint64_t guid = 0;

        if (fs::exists(metaPath)) {
            std::ifstream file(metaPath);
            if (file.is_open()) {
                try {
                    json data = json::parse(file);
                    if (data.contains("guid")) {
                        if (data["guid"].is_string()) {
                            // 文字列形式の場合（互換性用）
                            guid = std::stoull(data["guid"].get<std::string>());
                        } else {
                            guid = data["guid"].get<uint64_t>();
                        }
                    }
                } catch (...) {}
            }
        }

        if (guid == 0) {
            guid = GenerateGuid();
            SaveMeta(path, guid);
            Engine::Console::Log(std::format("AssetDatabase: Generated new meta for {}", path.string()));
        }

        std::string relPath = fs::relative(path, projectRoot).string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        // Project/ を削って統一
        if (relPath.substr(0, 8) == "Project/") {
            relPath = relPath.substr(8);
        }

        pathToGuid_[relPath] = guid;
        guidToPath_[guid] = relPath;
	}

	Engine::Console::Log(std::format("AssetDatabase: {} assets registered.", pathToGuid_.size()));
}

uint64_t AssetDatabase::GetGuidFromPath(const std::string& path) {
	std::string normalizedPath = path;
	std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');

    // 先頭の ./ を削除
    if (normalizedPath.substr(0, 2) == "./") {
        normalizedPath = normalizedPath.substr(2);
    }

	if(normalizedPath.substr(0, 8) == "Project/") {
		normalizedPath = normalizedPath.substr(8);
	}

	auto it = pathToGuid_.find(normalizedPath);
    if (it == pathToGuid_.end()) {
        Engine::Console::LogWarning(std::format("AssetDatabase: GUID not found for path: {}", normalizedPath));
        return 0;
    }
	return it->second;
}

std::string AssetDatabase::GetPathFromGuid(uint64_t guid) {
	auto it = guidToPath_.find(guid);
	return (it != guidToPath_.end()) ? it->second : "";
}

uint64_t AssetDatabase::GenerateGuid() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis;
    
    uint64_t guid = 0;
    while (guid == 0 || guidToPath_.count(guid)) {
        guid = dis(gen);
    }
    return guid;
}

void AssetDatabase::SaveMeta(const std::filesystem::path& assetPath, uint64_t guid) {
    fs::path metaPath = assetPath.string() + ".meta";
    json data;
    data["guid"] = guid;
    
    std::ofstream file(metaPath);
    if (file.is_open()) {
        file << data.dump(4);
    }
}

} // namespace Engine::Asset
