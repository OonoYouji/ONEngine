#include "AssetType.h"

/// std
#include <unordered_map>

namespace {

std::unordered_map<std::string, ONEngine::Asset::AssetType> gAssetTypeCheckMap = {
	{ ".png", ONEngine::Asset::AssetType::Texture },
	{ ".jpeg", ONEngine::Asset::AssetType::Texture },
	{ ".jpg", ONEngine::Asset::AssetType::Texture },
	{ ".dds", ONEngine::Asset::AssetType::Texture },
	{ ".obj", ONEngine::Asset::AssetType::Mesh },
	{ ".gltf", ONEngine::Asset::AssetType::Mesh },
	{ ".mp3", ONEngine::Asset::AssetType::Audio },
	{ ".wav", ONEngine::Asset::AssetType::Audio },
	{ ".mate", ONEngine::Asset::AssetType::Material },
	{ ".hlsl", ONEngine::Asset::AssetType::Shader },
};

}	/// namespace


namespace ONEngine::Asset {

bool CheckAssetType(const std::string& _extension, AssetType _type) {
	/// ----- 引数の拡張子がアセットの物か確認する ----- ///

	if(gAssetTypeCheckMap.contains(_extension)) {
		AssetType type = gAssetTypeCheckMap[_extension];
		return type == _type;
	}

	return false;
}


AssetType GetAssetTypeFromExtension(const std::string& _extension) {
	/// ----- 引数がどのアセットか返す(無効な拡張子であればNoneを) ----- ///

	if(gAssetTypeCheckMap.contains(_extension)) {
		return gAssetTypeCheckMap[_extension];
	}
	return AssetType::None;
}

}
