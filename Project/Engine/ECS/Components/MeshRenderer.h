#pragma once

#include <string>
#include <cstdint>

namespace Engine::ECS {

///
/// メッシュレンダラーコンポーネント
///
struct MeshRenderer {
	std::string modelName;    // AssetManagerに登録されたモデル名
	std::string materialName; // MaterialManagerに登録されたマテリアル名
};

} // namespace Engine::ECS
