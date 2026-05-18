#include "Engine/ECS/Registry.h"
#include "Engine/ECS/ComponentRegistry.h"
#include "Engine/Graphics/PostProcess/DebugRenderer.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"
#include <cstdint>

// Linker Force: エクスポートを確実にするための空関数
extern "C" {
    __declspec(dllexport) void EcsInterop_LinkForce() {}

    __declspec(dllexport) void* ecs_get_sparse_pages(Engine::ECS::Registry* registry, uint32_t typeId, uint32_t* pageCount) {
        auto* info = Engine::ECS::ComponentRegistry::GetInstance().GetInfo(typeId);
        if (!info) return nullptr;
        return info->getStorageFunc(*registry).GetSparsePagesPtr(pageCount);
    }

    __declspec(dllexport) void* ecs_get_chunk_ptr(Engine::ECS::Registry* registry, uint32_t typeId, uint32_t chunkIndex) {
        auto* info = Engine::ECS::ComponentRegistry::GetInstance().GetInfo(typeId);
        if (!info) return nullptr;
        return info->getStorageFunc(*registry).GetChunkPtr(chunkIndex);
    }

    __declspec(dllexport) uint32_t ecs_get_chunk_count(Engine::ECS::Registry* registry, uint32_t typeId) {
        auto* info = Engine::ECS::ComponentRegistry::GetInstance().GetInfo(typeId);
        if (!info) return 0;
        return (uint32_t)((info->getStorageFunc(*registry).Size() + 1023) / 1024);
    }

    __declspec(dllexport) uint32_t ecs_get_storage_size(Engine::ECS::Registry* registry, uint32_t typeId) {
        auto* info = Engine::ECS::ComponentRegistry::GetInstance().GetInfo(typeId);
        if (!info) return 0;
        return (uint32_t)info->getStorageFunc(*registry).Size();
    }

    __declspec(dllexport) uint32_t CreateEntity(Engine::ECS::Registry* registry) {
        return registry->CreateEntity();
    }

    __declspec(dllexport) void DestroyEntity(Engine::ECS::Registry* registry, uint32_t entity) {
        registry->DestroyEntity(entity);
    }

    __declspec(dllexport) void AddTransform(Engine::ECS::Registry* registry, uint32_t entity) {
        registry->AddComponent<Engine::ECS::Transform>(entity);
    }

    __declspec(dllexport) void AddMeshRenderer(Engine::ECS::Registry* registry, uint32_t entity) {
        registry->AddComponent<Engine::ECS::MeshRenderer>(entity);
    }

    __declspec(dllexport) void AddScriptComponent(Engine::ECS::Registry* registry, uint32_t entity, uint64_t gcHandle, uint32_t typeId) {
        Engine::ECS::ScriptComponent sc;
        sc.isEnabled = 1;
        sc.gcHandle = gcHandle;
        sc.typeId = typeId;
        registry->AddComponent<Engine::ECS::ScriptComponent>(entity, std::move(sc));
    }

    __declspec(dllexport) void Debug_DrawLine(float sx, float sy, float sz, float ex, float ey, float ez, float r, float g, float b, float a) {
        Engine::Graphics::DebugRenderer::GetInstance().DrawLine({sx, sy, sz}, {ex, ey, ez}, {r, g, b, a});
    }

    // 型IDのエクスポート (定数)
    __declspec(dllexport) uint32_t Ecs_GetTypeId_Transform() { return 1; }
    __declspec(dllexport) uint32_t Ecs_GetTypeId_MeshRenderer() { return 2; }
    __declspec(dllexport) uint32_t Ecs_GetTypeId_ScriptComponent() { return 3; }
}
