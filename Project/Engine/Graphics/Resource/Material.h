#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "Engine/Core/Math/Vector4.h"

namespace Engine::Graphics {

///
/// マテリアルアセット
///
struct Material {
    std::string name;
    std::string pipelineName; // 使用するパイプライン
    std::string textureName;  // メインテクスチャ名
    Engine::Math::Vector4 baseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
};

} // namespace Engine::Graphics
