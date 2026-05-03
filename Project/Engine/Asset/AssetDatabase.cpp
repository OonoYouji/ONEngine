#include "AssetDatabase.h"
#include "Externals/nlohmann/json.hpp"
#include "Engine/Common/Console.h"
#include <fstream>
#include <algorithm>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace Engine::Asset {

void AssetDatabase::Scan(const std::string& rootDir) {
	if(!fs::exists(rootDir)) return;

	Engine::Console::Log(std::format("Scanning assets in: {}", rootDir));

	// プロジェクトルート（Projectフォルダ）の絶対パスを取得
	fs::path projectRoot = fs::current_path();

	for(const auto& entry : fs::recursive_directory_iterator(rootDir)) {
		if(entry.path().extension() == ".meta") {
			std::ifstream file(entry.path());
			if(!file.is_open()) continue;

			try {
				json data = json::parse(file);
				if(data.contains("guid")) {
					std::string guid = data["guid"];

					// .metaを除いた本来のアセットパスを取得
					fs::path fullPath = entry.path();
					std::string fullPathStr = fullPath.string();
					std::string assetPathStr = fullPathStr.substr(0, fullPathStr.size() - 5);
					fs::path assetPath(assetPathStr);

					// プロジェクトルートからの相対パスに変換 (例: Assets/Textures/Grid.png)
					std::string relPath = fs::relative(assetPath, projectRoot).string();
					std::replace(relPath.begin(), relPath.end(), '\\', '/');

					pathToGuid_[relPath] = guid;
					guidToPath_[guid] = relPath;
				}
			} catch(...) {
				// Ignore
			}
		}
	}

	Engine::Console::Log(std::format("AssetDatabase: {} assets registered.", pathToGuid_.size()));
}

std::string AssetDatabase::GetGuidFromPath(const std::string& path) const {
	std::string normalizedPath = path;
	std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');

	// もし "Project/" から始まっていたら取り除く
	if(normalizedPath.substr(0, 8) == "Project/") {
		normalizedPath = normalizedPath.substr(8);
	}

	auto it = pathToGuid_.find(normalizedPath);
	return (it != pathToGuid_.end()) ? it->second : "";
}

std::string AssetDatabase::GetPathFromGuid(const std::string& guid) const {
	auto it = guidToPath_.find(guid);
	return (it != guidToPath_.end()) ? it->second : "";
}

void AssetDatabase::Dump() const {
	Engine::Console::Log("--- AssetDatabase Dump ---");
	for(const auto& [path, guid] : pathToGuid_) {
		Engine::Console::Log(std::format("  {} -> {}", path, guid));
	}
}

} // namespace Engine::Asset
