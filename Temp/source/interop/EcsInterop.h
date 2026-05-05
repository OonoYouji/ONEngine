#pragma once
#include <cstdint>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

class EcsWorld;

extern "C" {
    // --- ワールド管理 ---
    EXPORT EcsWorld* ecs_world_create();
    EXPORT void ecs_world_destroy(EcsWorld* world);
    EXPORT uint64_t ecs_create_entity(EcsWorld* world);
    EXPORT void ecs_destroy_entity(EcsWorld* world, uint64_t entity_id);
    EXPORT void ecs_submit_commands(EcsWorld* world);

    // --- 汎用コンポーネント操作 ---
    EXPORT void ecs_add_component(EcsWorld* world, uint64_t entity_id, uint32_t type_id, const void* data);
    EXPORT void ecs_remove_component(EcsWorld* world, uint64_t entity_id, uint32_t type_id);
    EXPORT void* ecs_get_component(EcsWorld* world, uint64_t entity_id, uint32_t type_id);

    // --- ページ・チャンクアクセス ---
    EXPORT void** ecs_get_sparse_pages(EcsWorld* world, uint32_t type_id, uint32_t* out_page_count);
    EXPORT void* ecs_get_chunk_ptr(EcsWorld* world, uint32_t type_id, uint32_t chunk_index);
    EXPORT uint32_t ecs_get_chunk_count(EcsWorld* world, uint32_t type_id);
    EXPORT uint32_t ecs_get_storage_size(EcsWorld* world, uint32_t type_id);
    EXPORT uint64_t* ecs_get_entities_ptr(EcsWorld* world, uint32_t type_id, uint32_t* out_count);

    // --- Cleanup & Destruction ---
    EXPORT void ecs_register_destruct_hook(EcsWorld* world, uint32_t type_id);
    EXPORT uint64_t* ecs_get_destroyed_entities(EcsWorld* world, uint32_t* out_count);
    EXPORT void ecs_clear_destroyed_entities(EcsWorld* world);
}
