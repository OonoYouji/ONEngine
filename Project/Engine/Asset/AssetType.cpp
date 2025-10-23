#include "AssetType.h"

/// std
#include <unordered_map>

namespace {

	std::unordered_map<std::string, AssetType> gAssetTypeCheckMap = {
		{ ".png", AssetType::Texture },
		{ ".jpeg", AssetType::Texture },
		{ ".jpg", AssetType::Texture },
		{ ".obj", AssetType::Mesh },
		{ ".gltf", AssetType::Mesh },
		{ ".mp3", AssetType::Audio },
		{ ".wav", AssetType::Audio },
		{ ".mate", AssetType::Material },
	};

}	/// namespace


bool CheckAssetType(const std::string& _extension, AssetType _type) {
	if (gAssetTypeCheckMap.contains(_extension)) {
		AssetType type = gAssetTypeCheckMap[_extension];
		return type == _type;
	}

	return false;
}
