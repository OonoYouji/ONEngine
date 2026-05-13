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
    std::function<json(Entity, Registry&)> serializeFunc;
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
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new ComponentRegistry();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    /// @brief 新しいコンポーネントを登録
    template <typename T>
    void Register(uint32_t typeId, const std::string& name, 
                  std::function<void(const json&, T&)> deserialize,
                  std::function<json(const T&)> serialize) {
        ComponentTypeInfo info;
        info.typeId = typeId;
        info.name = name;
        
        info.serializeFunc = [serialize](Entity e, Registry& r) -> json {
            if (!r.HasComponent<T>(e)) return json{};
            return serialize(r.GetComponent<T>(e));
        };

        info.deserializeFunc = [deserialize](const json& j, Entity e, Registry& r) {
            auto& comp = r.HasComponent<T>(e) ? r.GetComponent<T>(e) : r.AddComponent<T>(e);
            deserialize(j, comp);
        };
        
        info.getStorageFunc = [typeId](Registry& r) -> IComponentStorage& {
            auto& storage = r.GetStorage<T>();
            storage.SetTypeId(typeId); // Storage 側にも ID を伝播
            return storage;
        };

        info.addFunc = [typeId](Registry& r, Entity e) {
            auto& storage = r.GetStorage<T>();
            storage.SetTypeId(typeId);
            r.AddComponent<T>(e);
        };

        idToInfo_[typeId] = info;
        nameToInfo_[name] = info;
    }

    /// @brief コンポーネントを JSON にシリアライズ
    json SerializeComponent(Registry& reg, Entity entity, uint32_t typeId) {
        auto it = idToInfo_.find(typeId);
        if (it == idToInfo_.end()) return json{};
        return it->second.serializeFunc(entity, reg);
    }

    /// @brief JSON からコンポーネントをデシリアライズ
    void DeserializeComponent(Registry& reg, Entity entity, uint32_t typeId, const json& data) {
        auto it = idToInfo_.find(typeId);
        if (it != idToInfo_.end()) {
            it->second.deserializeFunc(data, entity, reg);
        }
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

    static ComponentRegistry* instance_;

    std::unordered_map<uint32_t, ComponentTypeInfo> idToInfo_;
    std::unordered_map<std::string, ComponentTypeInfo> nameToInfo_;
};

/// @brief 全てのコンポーネントをレジストリに登録
void InitializeComponentRegistry();

} // namespace Engine::ECS
