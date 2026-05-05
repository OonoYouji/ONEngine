#include <Windows.h>
#include <vector>
#include "Engine/Core/Window.h"
#include "Engine/Core/Timer.h"
#include "Engine/Script/ScriptHost.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Core/Renderer.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Asset/MaterialManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Engine/ECS/Registry.h"
#include "Engine/ECS/Systems/RenderSystem.h"
#include "Engine/ECS/Systems/CameraSystem.h"
#include "Engine/ECS/Systems/LightSystem.h"
#include "Engine/Common/Console.h"
#include "Engine/Scene/SceneLoader.h"
#include "Schema/Schema.h"

extern "C" void LogFromRuntime(const char*);

extern "C" {
    // --- Component Type IDs ---
    __declspec(dllexport) uint32_t Ecs_GetTypeId_Transform() { return 1; }
    __declspec(dllexport) uint32_t Ecs_GetTypeId_MeshRenderer() { return 2; }
    __declspec(dllexport) uint32_t Ecs_GetTypeId_ScriptComponent() { return 3; }

    // --- Generic Storage Access ---
    __declspec(dllexport) void* ecs_get_sparse_pages(Engine::ECS::Registry* registry, uint32_t typeId, uint32_t* pageCount) {
        if (typeId == 1) return registry->GetStorage<Engine::ECS::Transform>().GetSparsePagesPtr(pageCount);
        if (typeId == 2) return registry->GetStorage<Engine::ECS::MeshRenderer>().GetSparsePagesPtr(pageCount);
        if (typeId == 3) return registry->GetStorage<Engine::ECS::ScriptComponent>().GetSparsePagesPtr(pageCount);
        return nullptr;
    }

    __declspec(dllexport) void* ecs_get_chunk_ptr(Engine::ECS::Registry* registry, uint32_t typeId, uint32_t chunkIndex) {
        if (typeId == 1) return registry->GetStorage<Engine::ECS::Transform>().GetChunkPtr(chunkIndex);
        if (typeId == 2) return registry->GetStorage<Engine::ECS::MeshRenderer>().GetChunkPtr(chunkIndex);
        if (typeId == 3) return registry->GetStorage<Engine::ECS::ScriptComponent>().GetChunkPtr(chunkIndex);
        return nullptr;
    }

    __declspec(dllexport) uint32_t ecs_get_chunk_count(Engine::ECS::Registry* registry, uint32_t typeId) {
        if (typeId == 1) return (uint32_t)((registry->GetStorage<Engine::ECS::Transform>().Size() + 1023) / 1024);
        if (typeId == 2) return (uint32_t)((registry->GetStorage<Engine::ECS::MeshRenderer>().Size() + 1023) / 1024);
        if (typeId == 3) return (uint32_t)((registry->GetStorage<Engine::ECS::ScriptComponent>().Size() + 1023) / 1024);
        return 0;
    }

    __declspec(dllexport) uint32_t ecs_get_storage_size(Engine::ECS::Registry* registry, uint32_t typeId) {
        if (typeId == 1) return (uint32_t)registry->GetStorage<Engine::ECS::Transform>().Size();
        if (typeId == 2) return (uint32_t)registry->GetStorage<Engine::ECS::MeshRenderer>().Size();
        if (typeId == 3) return (uint32_t)registry->GetStorage<Engine::ECS::ScriptComponent>().Size();
        return 0;
    }

    __declspec(dllexport) uint32_t* ecs_get_entities_ptr(Engine::ECS::Registry* registry, uint32_t typeId, uint32_t* count) {
        if (typeId == 1) { 
            auto& s = registry->GetStorage<Engine::ECS::Transform>();
            *count = (uint32_t)s.GetEntities().size();
            return (uint32_t*)s.GetEntities().data();
        }
        if (typeId == 2) {
            auto& s = registry->GetStorage<Engine::ECS::MeshRenderer>();
            *count = (uint32_t)s.GetEntities().size();
            return (uint32_t*)s.GetEntities().data();
        }
        if (typeId == 3) {
            auto& s = registry->GetStorage<Engine::ECS::ScriptComponent>();
            *count = (uint32_t)s.GetEntities().size();
            return (uint32_t*)s.GetEntities().data();
        }
        return nullptr;
    }

    __declspec(dllexport) uint32_t GetEntityId(Engine::ECS::Registry* registry, uint32_t index) {
        auto& entities = registry->GetStorage<Engine::ECS::Transform>().GetEntities();
        if (index >= entities.size()) return 0;
        return entities[index];
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
        sc.gcHandle = gcHandle;
        sc.typeId = typeId;
        registry->AddComponent<Engine::ECS::ScriptComponent>(entity, std::move(sc));
    }
}

using namespace Engine::GeneratedSchema;
using namespace Engine::Asset;
using namespace Engine::ECS;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Engine::Console::Initialize();

    Engine::ECS::Registry registry;

    Engine::Core::Window window;
    window.Initialize(L"AssetSystemUnifiedTest", Engine::Math::Vector2Int::HD);

    auto& graphicsEngine = Engine::Graphics::GraphicsEngine::GetInstance();
    graphicsEngine.Initialize(window.GetHWND(), Engine::Math::Vector2Int::HD);

    auto& shaderManager = Engine::Graphics::ShaderManager::GetInstance();
    shaderManager.Initialize(graphicsEngine.GetRenderDevice());
    
    auto& textureManager = TextureManager::GetInstance();
    textureManager.Initialize(graphicsEngine.GetRenderDevice());

    auto& materialManager = MaterialManager::GetInstance();
    materialManager.Initialize(graphicsEngine.GetRenderDevice());

    auto& assetManager = AssetManager::GetInstance();
    assetManager.Initialize(graphicsEngine.GetRenderDevice());

    auto& scriptHost = Engine::Script::ScriptHost::GetInstance();
    void(*updateDelegate)(float) = nullptr;
    void(*shutdownDelegate)() = nullptr;

    if (scriptHost.Initialize()) {
        auto initDelegate = (void(*)(void*, void*))scriptHost.GetMethodDelegate(
            L"ONEngine.Scripting.EngineHost, ONEngine.Scripting",
            L"Initialize",
            L"");
        
        if (initDelegate) initDelegate((void*)LogFromRuntime, &registry);

        updateDelegate = (void(*)(float))scriptHost.GetMethodDelegate(
            L"ONEngine.Scripting.EngineHost, ONEngine.Scripting",
            L"Update",
            L"");

        shutdownDelegate = (void(*)())scriptHost.GetMethodDelegate(
            L"ONEngine.Scripting.EngineHost, ONEngine.Scripting",
            L"Shutdown",
            L"");
    }

    auto& renderer = Engine::Graphics::Renderer::GetInstance();
    renderer.Initialize(graphicsEngine.GetRenderDevice());

    shaderManager.LoadPipelineAsset("Assets/Pipelines/BindlessTest.json");
    shaderManager.LoadPipelineAsset("Assets/Pipelines/Blit.json");

    Engine::Scene::SceneLoader::LoadScene("Assets/Scene/Main.scene", registry);

    Engine::ECS::RenderSystem renderSystem;
    Engine::ECS::CameraSystem cameraSystem;
    Engine::ECS::LightSystem lightSystem;
    Engine::Graphics::ConstantBuffer sceneCB;
    sceneCB.Create(graphicsEngine.GetRenderDevice(), sizeof(SceneData));

    Engine::Core::Timer timer;
    timer.Reset();

    while(true) {
        window.Update();
        if(window.GetIsProcessEnd()) break;

        timer.Tick();
        float dt = timer.GetDeltaTime();
if (updateDelegate) updateDelegate(dt);

// システム更新
cameraSystem.Reset();
cameraSystem.Update(registry);

lightSystem.Reset();
lightSystem.Update(registry);

renderer.ClearQueue();
renderSystem.Update(registry);

// 描画実行
graphicsEngine.BeginFrame();

// カメラ・ライト・シーンデータの更新
SceneData sceneData;
if (cameraSystem.HasCamera()) {
    sceneData.viewProj = cameraSystem.GetResult().viewProj;
    sceneData.cameraPos = cameraSystem.GetResult().position;
} else {
    auto view = Engine::Math::Matrix4x4::MakeLookAtLH({ 0, 20, -50 }, { 0, 0, 0 }, { 0, 1, 0 });
    auto proj = Engine::Math::Matrix4x4::MakePerspectiveFovLH(0.45f, 16.0f/9.0f, 0.1f, 1000.0f);
    sceneData.viewProj = view * proj;
    sceneData.cameraPos = { 0, 20, -50 };
}

auto lightRes = lightSystem.GetResult();
sceneData.dirLightColor = lightRes.dirLightColor;
sceneData.dirLightIntensity = lightRes.dirLightIntensity;
sceneData.dirLightDirection = lightRes.dirLightDirection;
sceneData.padding = 0.0f;

auto* currentFrameRes = graphicsEngine.GetCurrentFrameResource();
currentFrameRes->GetSceneCB()->Update(&sceneData, sizeof(sceneData));


        renderer.Extract();
        
        graphicsEngine.Clear({ 0.7f, 0.7f, 0.7f, 1.0f });
        graphicsEngine.ClearDepth();
        
        Engine::Graphics::RenderContext context;
        context.commandList = graphicsEngine.GetCommandQueue()->GetCommandList();
        context.sceneCBAddress = currentFrameRes->GetSceneCB()->GetGPUVirtualAddress();
        context.frameIndex = graphicsEngine.GetCurrentFrameIndex();
        
        renderer.RenderZPrepass(context);
        renderer.Render(context);

        graphicsEngine.EndFrame();
    }

    renderer.Shutdown();
    if (shutdownDelegate) shutdownDelegate();
    scriptHost.Shutdown();
    assetManager.Shutdown();
    materialManager.Shutdown();
    textureManager.Shutdown();
    shaderManager.Shutdown();
    graphicsEngine.Shutdown();
    window.Shutdown();
    Engine::Console::Shutdown();

    return 0;
}
