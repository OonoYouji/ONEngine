#pragma once

#include <string>

namespace Engine::ECS {

///
/// メッシュレンダラーコンポーネント
///
struct MeshRenderer {
	std::string meshPath;
	// マテリアル等の情報も本来はここに入る
};

} // namespace Engine::ECS
