#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace Engine::Graphics {

///
/// マテリアルアセット
///
struct Material {
    std::string name;
    std::string pipelineName; // 使用するパイプライン（ShaderManagerに登録されているもの）
    std::string textureName;  // メインテクスチャ名（TextureManagerに登録されているもの）
    
    // 今後拡張：色のパラメータ、複数テクスチャ、等
};

} // namespace Engine::Graphics
