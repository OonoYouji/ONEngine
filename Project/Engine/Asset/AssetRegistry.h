#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <typeindex>
#include <functional>
#include <type_traits>

#include "IAsset.h"
#include "AssetHandle.h"

namespace Engine {

///
/// アセットの読み込みとキャッシュを統括するクラス
///
class AssetRegistry {
public:
    static AssetRegistry& GetInstance() {
        static AssetRegistry instance;
        return instance;
    }

    /// @brief アセットのロード
    /// @tparam T アセットの型
    /// @param pathOrGuid パスまたはGUID
    /// @return アセットへのハンドル
    template<typename T>
    AssetHandle<T> Load(const std::string& pathOrGuid) {
        // すでにロード済みかチェック
        auto it = assets_.find(pathOrGuid);
        if (it != assets_.end()) {
            return AssetHandle<T>(std::static_pointer_cast<T>(it->second));
        }

        // 型に対応するアセット種別を取得
        auto typeIt = typeToAssetType_.find(typeid(T));
        if (typeIt == typeToAssetType_.end()) return AssetHandle<T>();

        AssetType type = typeIt->second;
        auto loaderIt = loaders_.find(type);
        if (loaderIt != loaders_.end()) {
            auto asset = loaderIt->second(pathOrGuid);
            if (asset) {
                assets_[pathOrGuid] = asset;
                return AssetHandle<T>(std::static_pointer_cast<T>(asset));
            }
        }

        return AssetHandle<T>();
    }

    /// @brief 型とアセット種別の紐付け
    template<typename T>
    void RegisterType(AssetType type) {
        typeToAssetType_[typeid(T)] = type;
    }

    /// @brief ローダーの登録
    void RegisterLoader(AssetType type, std::function<std::shared_ptr<IAsset>(const std::string&)> loader) {
        loaders_[type] = loader;
    }

private:
    AssetRegistry() = default;
    ~AssetRegistry() = default;

private:
    // GUID または Path をキーにした全アセットのキャッシュ
    std::unordered_map<std::string, std::shared_ptr<IAsset>> assets_;
    // アセット種別ごとのローダー
    std::unordered_map<AssetType, std::function<std::shared_ptr<IAsset>(const std::string&)>> loaders_;
    // 型とアセット種別のマップ
    std::unordered_map<std::type_index, AssetType> typeToAssetType_;
};

} // namespace Engine
