#pragma once

/// std
#include <algorithm>
#include <string>

/// engine
#include "Engine/Asset/Guid/Guid.h"

/// @brief Assetのペイロード (string + Guid)
namespace ONEngine {

struct AssetPayload {
	std::string filePath;
	Guid        guid;

	AssetPayload() = default;
	AssetPayload(const std::string& _path, const Guid& _guid)
		: filePath(_path), guid(_guid) {
	}

};

} /// ONEngine
