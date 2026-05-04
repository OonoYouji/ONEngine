#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "Engine/Core/Math/Vector4.h"
#include "IAsset.h"

namespace Engine::Asset {

///
/// マテリアルアセット
///
class Material : public IAsset {
public:
    virtual ~Material() = default;

    /// @brief IAssetの実装
    AssetType GetType() const override { return AssetType::Material; }

    std::string name;
    std::string pipelineName; // 使用するパイプライン
    uint32_t textureIndex = 0xFFFFFFFF; // メインテクスチャインデックス
    Engine::Math::Vector4 baseColor = { 1.0f, 1.0f, 1.0f, 1.0f };

    // ロード成功時に呼ぶ
    void OnLoaded() { state_ = AssetLoadState::Ready; }
};

} // namespace Engine::Asset

