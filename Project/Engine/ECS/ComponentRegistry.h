#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "Registry.h"
#include "Externals/nlohmann/json.hpp"

namespace Engine::ECS {

using json = nlohmann::json;

///
/// コンポーネントの種類ごとの情報を保持する
///
struct ComponentTypeInfo {
    uint32_t typeId;
    std::string name;
    std::function<void(const json&, Entity, Registry&)> deserializeFunc;
    std::function<IComponentStorage&(Registry&)> getStorageFunc;
    std::function<void(Registry&, Entity)> addFunc;
};

///
/// 全てのコンポーネントを管理・登録するクラス
///
class ComponentRegistry {
public:
    static ComponentRegistry& GetInstance() {
        static ComponentRegistry instance;
        return instance;
    }

    /// @brief 新しいコンポーネントを登録
    template <typename T>
    void Register(uint32_t typeId, const std::string& name, std::function<void(const json&, T&)> deserialize) {
        ComponentTypeInfo info;
        info.typeId = typeId;
        info.name = name;
        
        info.deserializeFunc = [deserialize](const json& j, Entity e, Registry& r) {
            auto& comp = r.AddComponent<T>(e);
            deserialize(j, comp);
        };
        
        info.getStorageFunc = [](Registry& r) -> IComponentStorage& {
            return r.GetStorage<T>();
        };

        info.addFunc = [](Registry& r, Entity e) {
            r.AddComponent<T>(e);
        };

        idToInfo_[typeId] = info;
        nameToInfo_[name] = info;
    }

    const ComponentTypeInfo* GetInfo(uint32_t typeId) const {
        auto it = idToInfo_.find(typeId);
        return (it != idToInfo_.end()) ? &it->second : nullptr;
    }

    const ComponentTypeInfo* GetInfo(const std::string& name) const {
        auto it = nameToInfo_.find(name);
        return (it != nameToInfo_.end()) ? &it->second : nullptr;
    }

    const std::unordered_map<uint32_t, ComponentTypeInfo>& GetAll() const { return idToInfo_; }

private:
    ComponentRegistry() = default;

    std::unordered_map<uint32_t, ComponentTypeInfo> idToInfo_;
    std::unordered_map<std::string, ComponentTypeInfo> nameToInfo_;
};

/// @brief 全てのコンポーネントをレジストリに登録
void InitializeComponentRegistry();

} // namespace Engine::ECS
