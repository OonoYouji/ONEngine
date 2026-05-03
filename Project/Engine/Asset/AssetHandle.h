#pragma once

#include <memory>
#include "IAsset.h"

namespace Engine {

///
/// アセットを安全に参照するためのハンドルクラス
///
template<typename T>
class AssetHandle {
public:
    AssetHandle() = default;
    AssetHandle(std::shared_ptr<T> asset) : asset_(asset) {}

    /// @brief アセットが有効か（ロード済みでエラーがないか）
    bool IsValid() const {
        return asset_ && asset_->GetLoadState() == AssetLoadState::Ready;
    }

    /// @brief ロード中かどうか
    bool IsLoading() const {
        return asset_ && asset_->GetLoadState() == AssetLoadState::Loading;
    }

    /// @brief アセットへのアクセス
    T* operator->() const { return asset_.get(); }
    T& operator*() const { return *asset_; }

    /// @brief 生ポインタを取得
    T* Get() const { return asset_.get(); }

    /// @brief handleが空かどうか
    bool IsNull() const { return asset_ == nullptr; }

    /// @brief 型をキャストして新しいハンドルを返す
    template<typename U>
    AssetHandle<U> As() const {
        return AssetHandle<U>(std::static_pointer_cast<U>(asset_));
    }

private:
    std::shared_ptr<T> asset_;
};

} // namespace Engine
