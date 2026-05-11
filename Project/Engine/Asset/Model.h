#pragma once

#include <vector>
#include <memory>
#include "IAsset.h"
#include "Mesh.h"
#include "Animation.h"

namespace Engine::Asset {

///
/// モデルアセット（複数のメッシュで構成される）
///
class Model : public IAsset {
public:
    Model() = default;
    virtual ~Model() = default;

    /// @brief IAssetの実装
    AssetType GetType() const override { return AssetType::Model; }

    /// @brief メッシュリストを設定
    void SetMeshes(std::vector<std::unique_ptr<Mesh>> meshes) {
        meshes_ = std::move(meshes);
        state_ = AssetLoadState::Ready;
    }

    /// @brief メッシュリストを取得
    const std::vector<std::unique_ptr<Mesh>>& GetMeshes() const { return meshes_; }

    /// @brief アニメーションを追加
    void AddAnimation(Animation anim) {
        animations_.push_back(std::move(anim));
    }

    /// @brief アニメーションリストを取得
    const std::vector<Animation>& GetAnimations() const { return animations_; }

private:
    std::vector<std::unique_ptr<Mesh>> meshes_;
    std::vector<Animation> animations_;
};

} // namespace Engine::Asset
