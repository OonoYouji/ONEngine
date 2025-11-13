#include "AssetType.h"

/// std
#include <unordered_map>

namespace {

	std::unordered_map<std::string, AssetType> gAssetTypeCheckMap = {
		{ ".png", AssetType::Texture },
		{ ".jpeg", AssetType::Texture },
		{ ".jpg", AssetType::Texture },
		{ ".dds", AssetType::Texture },
		{ ".obj", AssetType::Mesh },
		{ ".gltf", AssetType::Mesh },
		{ ".mp3", AssetType::Audio },
		{ ".wav", AssetType::Audio },
		{ ".mate", AssetType::Material },
	};

}	/// namespace


bool CheckAssetType(const std::string& _extension, AssetType _type) {
	/// ----- 引数の拡張子がアセットの物か確認する ----- ///

	if (gAssetTypeCheckMap.contains(_extension)) {
		AssetType type = gAssetTypeCheckMap[_extension];
		return type == _type;
	}

	return false;
}


AssetType GetAssetTypeFromExtension(const std::string& _extension) {
	/// ----- 引数がどのアセットか返す(無効な拡張子であればNoneを) ----- ///

	if (gAssetTypeCheckMap.contains(_extension)) {
		return gAssetTypeCheckMap[_extension];
	}
	return AssetType::None;
}