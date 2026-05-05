/**
 * @file EcsWorld.h
 * @brief Main ECS world managing entities and component storages.
 */

#pragma once
#include <vector>
#include <memory>
#include <deque>
#include <functional>
#include <tuple>
#include <assert.h>
#include "EcsCore.h"
#include "SparseSet.h"

class EcsWorld {
    struct EntityMeta {
        uint32_t generation;
        bool alive;
    };

    std::vector<EntityMeta> entities;
    std::vector<uint32_t> free_entities;
    
    // TypeID based storage access
    std::vector<std::unique_ptr<ISparseSet>> storages;

    // Destruction queue for cleanup phases (e.g., C# GCHandle freeing)
    std::vector<Entity> destroyed_queue;

    // Command buffer for deferred structural changes
    std::deque<std::function<void()>> command_buffer;

public:
    EcsWorld() {
        // Reserve index 0 if needed, or ensure it's handled. 
        // Here we just ensure the vector isn't empty to avoid index issues.
        entities.reserve(1024);
    }

    /**
     * @brief Registers a removal hook for a component type.
     * Useful for cleanup logic (e.g., in interop layers).
     */
    void register_destruct_hook(uint32_t type_id) {
        auto storage = get_storage_by_id(type_id);
        if (!storage) return;
        storage->set_on_remove_callback([this](Entity e) {
            destroyed_queue.push_back(e);
        });
    }

    const std::vector<Entity>& get_destroyed_entities() const {
        return destroyed_queue;
    }

    void clear_destroyed_entities() {
        destroyed_queue.clear();
    }

    Entity create_entity() {
        if (!free_entities.empty()) {
            uint32_t index = free_entities.back();
            free_entities.pop_back();
            entities[index].alive = true;
            return Entity(index, entities[index].generation);
        }
        uint32_t index = (uint32_t)entities.size();
        entities.push_back({ 0, true });
        return Entity(index, 0);
    }

    void destroy_entity(Entity entity) {
        if (!is_alive(entity)) return;
        
        command_buffer.push_back([this, entity]() {
            if (!is_alive(entity)) return;
            
            // Remove from all storages
            for (auto& storage : storages) {
                if (storage) storage->remove(entity);
            }
            
            auto& meta = entities[entity.index()];
            meta.generation++;
            meta.alive = false;
            free_entities.push_back(entity.index());
        });
    }

    bool is_alive(Entity entity) const {
        if (entity.index() >= entities.size()) return false;
        const auto& meta = entities[entity.index()];
        return meta.alive && meta.generation == entity.generation();
    }

    /**
     * @brief Gets or creates storage for a component type.
     */
    template<typename T>
    SparseSet<T>* get_storage() {
        uint32_t id = TypeID::get<T>();
        if (id >= storages.size()) {
            storages.resize(id + 1);
        }
        if (!storages[id]) {
            storages[id] = std::make_unique<SparseSet<T>>();
        }
        return static_cast<SparseSet<T>*>(storages[id].get());
    }

    /**
     * @brief Gets storage by TypeID (used for Interop).
     */
    ISparseSet* get_storage_by_id(uint32_t type_id) {
        if (type_id >= storages.size()) {
            storages.resize(type_id + 1);
        }
        if (!storages[type_id]) {
            storages[type_id] = TypeRegistry::Create(type_id);
        }
        return storages[type_id].get();
    }

    template<typename T>
    void add_component(Entity entity, T value) {
        command_buffer.push_back([this, entity, val = std::move(value)]() mutable {
            if (!is_alive(entity)) return;
            get_storage<T>()->add(entity, std::move(val));
        });
    }

    template<typename T>
    void remove_component(Entity entity) {
        command_buffer.push_back([this, entity]() {
            if (!is_alive(entity)) return;
            get_storage<T>()->remove(entity);
        });
    }
    
    template<typename T>
    bool has_component(Entity entity) {
        if (!is_alive(entity)) return false;
        return get_storage<T>()->contains(entity);
    }

    template<typename T>
    T& get_component(Entity entity) {
        assert(is_alive(entity));
        return get_storage<T>()->get(entity);
    }

    void submit_commands() {
        while (!command_buffer.empty()) {
            auto cmd = std::move(command_buffer.front());
            command_buffer.pop_front();
            cmd();
        }
    }

    // --- Query System ---

    template<typename... E>
    struct Exclude {};

    template<typename RequiredTuple, typename ExcludedTuple>
    struct View;

    template<typename... R, typename... E>
    struct View<std::tuple<R...>, std::tuple<E...>> {
        EcsWorld* world;

        template<typename Func>
        void each(Func func) {
            ISparseSet* smallest = nullptr;
            size_t min_size = (size_t)-1;

            auto r_storages = std::make_tuple(world->get_storage<R>()...);

            // Find the smallest storage for the base iteration
            auto find_min = [&](auto s) {
                if (s->size() < min_size) {
                    min_size = s->size();
                    smallest = s;
                }
            };
            std::apply([&](auto... s) { (find_min(s), ...); }, r_storages);

            if (!smallest || smallest->size() == 0) return;

            for (Entity entity : smallest->get_entities()) {
                // Must have all required
                if (!(world->get_storage<R>()->contains(entity) && ...)) continue;

                // Must have none excluded
                if constexpr (sizeof...(E) > 0) {
                    if ((world->get_storage<E>()->contains(entity) || ...)) continue;
                }

                if constexpr (sizeof...(R) > 0) {
                    func(entity, world->get_storage<R>()->get(entity)...);
                } else {
                    func(entity);
                }
            }
        }
    };

    template<typename... E>
    static Exclude<E...> exclude() { return Exclude<E...>{}; }

    template<typename... R, typename... E>
    auto view(Exclude<E...> = Exclude<>{}) {
        return View<std::tuple<R...>, std::tuple<E...>>{ this };
    }
};
