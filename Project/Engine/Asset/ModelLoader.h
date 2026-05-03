#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Mesh.h"

namespace Engine::Asset {

///
/// 3Dモデルをロードするためのクラス (Assimp使用)
///
class ModelLoader {
public:
    /// @brief ファイルからメッシュをロードする
    /// @param device デバイス
    /// @param filePath モデルファイルのパス
    /// @return ロードされたメッシュのリスト（サブメッシュ対応のため vector）
    static std::vector<std::unique_ptr<Mesh>> LoadModel(Graphics::RenderDevice* device, const std::string& filePath);
};

} // namespace Engine::Asset
