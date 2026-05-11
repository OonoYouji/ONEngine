#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include "IAsset.h"

namespace Engine::Asset {

///
/// アセットの型とローダーを登録・管理するレジストリ
///
class AssetRegistry {
public:
    static AssetRegistry& GetInstance() {
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new AssetRegistry();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    using LoaderFunc = std::function<std::shared_ptr<IAsset>(const std::string&)>;

    template <typename T>
    void RegisterType(AssetType type) {
        typeNames_[type] = typeid(T).name();
    }

    void RegisterLoader(AssetType type, LoaderFunc loader) {
        loaders_[type] = loader;
    }

    std::shared_ptr<IAsset> Load(AssetType type, const std::string& pathOrGuid) {
        if (loaders_.count(type)) {
            return loaders_[type](pathOrGuid);
        }
        return nullptr;
    }

private:
    AssetRegistry() = default;
    ~AssetRegistry() = default;

    static AssetRegistry* instance_;

    std::unordered_map<AssetType, std::string> typeNames_;
    std::unordered_map<AssetType, LoaderFunc> loaders_;
};

} // namespace Engine::Asset
