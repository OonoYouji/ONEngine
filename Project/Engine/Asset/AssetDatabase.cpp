#include "AssetDatabase.h"
#include "Externals/nlohmann/json.hpp"
#include "Engine/Common/Console.h"
#include <fstream>
#include <algorithm>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace Engine::Asset {

AssetDatabase* AssetDatabase::instance_ = nullptr;

void AssetDatabase::Scan(const std::string& directory) {
	if(!fs::exists(directory)) return;

	Engine::Console::Log(std::format("Scanning assets in: {}", directory));

	fs::path projectRoot = fs::current_path();

	for(const auto& entry : fs::recursive_directory_iterator(directory)) {
		if(entry.path().extension() == ".meta") {
			std::ifstream file(entry.path());
			if(!file.is_open()) continue;

			try {
				json data = json::parse(file);
				if(data.contains("guid")) {
					std::string guid = data["guid"];
					fs::path fullPath = entry.path();
					std::string fullPathStr = fullPath.string();
					std::string assetPathStr = fullPathStr.substr(0, fullPathStr.size() - 5);
					fs::path assetPath(assetPathStr);

					std::string relPath = fs::relative(assetPath, projectRoot).string();
					std::replace(relPath.begin(), relPath.end(), '\\', '/');

					pathToGuid_[relPath] = guid;
					guidToPath_[guid] = relPath;
				}
			} catch(...) {}
		}
	}

	Engine::Console::Log(std::format("AssetDatabase: {} assets registered.", pathToGuid_.size()));
}

std::string AssetDatabase::GetGuidFromPath(const std::string& path) {
	std::string normalizedPath = path;
	std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');

	if(normalizedPath.substr(0, 8) == "Project/") {
		normalizedPath = normalizedPath.substr(8);
	}

	auto it = pathToGuid_.find(normalizedPath);
	return (it != pathToGuid_.end()) ? it->second : "";
}

std::string AssetDatabase::GetPathFromGuid(const std::string& guid) {
	auto it = guidToPath_.find(guid);
	return (it != guidToPath_.end()) ? it->second : "";
}

} // namespace Engine::Asset
