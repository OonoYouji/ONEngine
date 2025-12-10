#pragma once

/// std
#include <algorithm>
#include <string>

/// engine
#include "Engine/Asset/Guid/Guid.h"

/// @brief Assetのペイロード (string + Guid)
namespace Editor {

struct AssetPayload {
	std::string    filePath;
	ONEngine::Guid guid;

	AssetPayload() = default;
	AssetPayload(const std::string& _path, const ONEngine::Guid& _guid)
		: filePath(_path), guid(_guid) {
	}

};

} /// Editor
