#pragma once

#include <string>
#include <cstdint>

namespace Engine::ECS {

///
/// メッシュレンダラーコンポーネント
///
struct MeshRenderer {
	std::string meshPath;
	uint32_t textureIndex = 0;
};

} // namespace Engine::ECS
