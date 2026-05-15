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
        return ext == ".png" || ext == ".jpg" || ext == ".obj" || ext == ".fbx" || ext == ".mat" || ext == ".scene" || ext == ".ttf" || ext == ".pipeline";
    }
}

void AssetDatabase::Scan(const std::string& directory) {
    std::string targetDir = directory;
    
    // パスの正規化：Project/Assets -> Assets (もしカレントディレクトリが Project/ なら)
    if (!fs::exists(targetDir)) {
        if (targetDir.substr(0, 8) == "Project/") {
            std::string altDir = targetDir.substr(8);
            if (fs::exists(altDir)) {
                targetDir = altDir;
            }
        }
    }

	Engine::Console::Log(std::format("********** AssetDatabase: Scan Started for [{}] **********", targetDir));
    
    if(!fs::exists(targetDir)) {
        Engine::Console::LogError(std::format("AssetDatabase: CRITICAL - Directory NOT FOUND: {}. Current path: {}", targetDir, fs::current_path().string()));
        return;
    }

	fs::path projectRoot = fs::current_path();
    int count = 0;
    int metaCount = 0;

	for(const auto& entry : fs::recursive_directory_iterator(targetDir)) {
        if (entry.is_directory()) continue;
        
        fs::path path = entry.path();
        if (path.extension() == ".meta") continue;
        if (!IsAssetFile(path)) continue;

        count++;
        fs::path metaPath = path.string() + ".meta";
        uint64_t guid = 0;

        // .meta の読み込み
        if (fs::exists(metaPath)) {
            std::ifstream file(metaPath);
            if (file.is_open()) {
                try {
                    json data = json::parse(file);
                    if (data.contains("guid")) {
                        if (data["guid"].is_string()) {
                            guid = std::stoull(data["guid"].get<std::string>());
                        } else {
                            guid = data["guid"].get<uint64_t>();
                        }
                    }
                } catch (...) {}
            }
        }

        // GUIDがなければ生成
        if (guid == 0) {
            guid = GenerateGuid();
            SaveMeta(path, guid);
            metaCount++;
            Engine::Console::Log(std::format("AssetDatabase: [CREATED] new meta for {}", path.string()));
        }

        // マップへの登録パスを正規化 (Assets/... 形式に統一)
        std::string relPath = fs::relative(path, projectRoot).string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        if (relPath.substr(0, 8) == "Project/") {
            relPath = relPath.substr(8);
        }

        pathToGuid_[relPath] = guid;
        guidToPath_[guid] = relPath;
	}

	Engine::Console::Log(std::format("********** AssetDatabase: Scan Complete. {} assets, {} new metas. **********", count, metaCount));
}

uint64_t AssetDatabase::GetGuidFromPath(const std::string& path) {
	std::string normalizedPath = path;
	std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');

    // 先頭の "./" や "/" を削除
    while (normalizedPath.length() >= 2 && normalizedPath.substr(0, 2) == "./") {
        normalizedPath = normalizedPath.substr(2);
    }
    if (!normalizedPath.empty() && normalizedPath[0] == '/') {
        normalizedPath = normalizedPath.substr(1);
    }

	if(normalizedPath.substr(0, 8) == "Project/") {
		normalizedPath = normalizedPath.substr(8);
	}

	auto it = pathToGuid_.find(normalizedPath);
    if (it != pathToGuid_.end()) {
        return it->second;
    }

    // fallback: Assets/ が抜けている可能性を考慮
    if (normalizedPath.find("Assets/") != 0) {
        std::string altPath = "Assets/" + normalizedPath;
        it = pathToGuid_.find(altPath);
        if (it != pathToGuid_.end()) return it->second;
    }

    Engine::Console::LogWarning(std::format("AssetDatabase: GUID not found for path: {}", normalizedPath));
	return 0;
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
