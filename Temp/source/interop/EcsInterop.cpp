#include "EcsInterop.h"
#include "EcsWorld.h"
#include "GeneratedComponents.h"

extern "C" {
    // --- ワールド管理 ---
    EXPORT EcsWorld* ecs_world_create() {
        return new EcsWorld();
    }

    EXPORT void ecs_world_destroy(EcsWorld* world) {
        delete world;
    }

    EXPORT uint64_t ecs_create_entity(EcsWorld* world) {
        return world->create_entity().id;
    }

    EXPORT void ecs_destroy_entity(EcsWorld* world, uint64_t entity_id) {
        world->destroy_entity(Entity( (uint32_t)(entity_id & 0xFFFFFFFF), (uint32_t)(entity_id >> 32) ));
    }

    EXPORT void ecs_submit_commands(EcsWorld* world) {
        world->submit_commands();
    }

    // --- 汎用コンポーネント操作 (Thin ABI) ---
    EXPORT void ecs_add_component(EcsWorld* world, uint64_t entity_id, uint32_t type_id, const void* data) {
        Entity entity((uint32_t)(entity_id & 0xFFFFFFFF), (uint32_t)(entity_id >> 32));
        auto storage = world->get_storage_by_id(type_id);
        if (storage) {
            storage->add_raw(entity, data);
        }
    }

    EXPORT void ecs_remove_component(EcsWorld* world, uint64_t entity_id, uint32_t type_id) {
        Entity entity((uint32_t)(entity_id & 0xFFFFFFFF), (uint32_t)(entity_id >> 32));
        auto storage = world->get_storage_by_id(type_id);
        if (storage) {
            storage->remove(entity);
        }
    }

    EXPORT void* ecs_get_component(EcsWorld* world, uint64_t entity_id, uint32_t type_id) {
        Entity entity((uint32_t)(entity_id & 0xFFFFFFFF), (uint32_t)(entity_id >> 32));
        auto storage = world->get_storage_by_id(type_id);
        if (storage && storage->contains(entity)) {
            return storage->get_raw(entity);
        }
        return nullptr;
    }

    // --- チャンク・イテレータ & ページアクセス ---
    EXPORT void** ecs_get_sparse_pages(EcsWorld* world, uint32_t type_id, uint32_t* out_page_count) {
        auto storage = world->get_storage_by_id(type_id);
        if (storage) {
            *out_page_count = (uint32_t)storage->get_sparse_page_count();
            return (void**)storage->get_sparse_pages_ptr();
        }
        return nullptr;
    }

    EXPORT void* ecs_get_chunk_ptr(EcsWorld* world, uint32_t type_id, uint32_t chunk_index) {
        auto storage = world->get_storage_by_id(type_id);
        if (storage) {
            return storage->get_chunk_ptr(chunk_index);
        }
        return nullptr;
    }

    EXPORT uint32_t ecs_get_chunk_count(EcsWorld* world, uint32_t type_id) {
        auto storage = world->get_storage_by_id(type_id);
        if (storage) {
            return (uint32_t)storage->get_chunk_count();
        }
        return 0;
    }

    EXPORT uint32_t ecs_get_storage_size(EcsWorld* world, uint32_t type_id) {
        auto storage = world->get_storage_by_id(type_id);
        if (storage) {
            return (uint32_t)storage->size();
        }
        return 0;
    }

    EXPORT uint64_t* ecs_get_entities_ptr(EcsWorld* world, uint32_t type_id, uint32_t* out_count) {
        auto storage = world->get_storage_by_id(type_id);
        if (storage) {
            *out_count = (uint32_t)storage->size();
            if (*out_count == 0) return nullptr;
            return (uint64_t*)storage->get_entities().data();
        }
        return nullptr;
    }

    // --- Cleanup & Destruction ---
    EXPORT void ecs_register_destruct_hook(EcsWorld* world, uint32_t type_id) {
        world->register_destruct_hook(type_id);
    }

    EXPORT uint64_t* ecs_get_destroyed_entities(EcsWorld* world, uint32_t* out_count) {
        const auto& destroyed = world->get_destroyed_entities();
        *out_count = (uint32_t)destroyed.size();
        if (destroyed.empty()) return nullptr;
        return (uint64_t*)destroyed.data();
    }

    EXPORT void ecs_clear_destroyed_entities(EcsWorld* world) {
        world->clear_destroyed_entities();
    }
}
