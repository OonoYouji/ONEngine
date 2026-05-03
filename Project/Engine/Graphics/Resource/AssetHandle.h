#pragma once

#include <string>
#include <memory>

namespace Engine {

///
/// アセットへの参照を安全に保持するハンドル
///
template<typename T>
class AssetHandle {
public:
    AssetHandle() : guid_(""), ptr_(nullptr) {}
    AssetHandle(const std::string& guid, T* ptr) : guid_(guid), ptr_(ptr) {}

    /// @brief アセットが有効か確認
    bool IsValid() const { return ptr_ != nullptr; }

    /// @brief アセットへのアクセス
    T* operator->() const { return ptr_; }
    T* Get() const { return ptr_; }

    const std::string& GetGuid() const { return guid_; }

    /// @brief ハンドルを無効化
    void Reset() {
        guid_ = "";
        ptr_ = nullptr;
    }

private:
    std::string guid_;
    T* ptr_ = nullptr;
};

} // namespace Engine
