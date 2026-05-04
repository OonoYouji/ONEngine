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
#include "Engine/Common/Console.h"
#include "Schema/Schema.h"

extern "C" void LogFromRuntime(const char*);

extern "C" {
    __declspec(dllexport) void* GetTransformChunk(Engine::ECS::Registry* registry, uint32_t chunkIndex) {
        return registry->GetStorage<Engine::ECS::Transform>().GetChunkPtr(chunkIndex);
    }

    __declspec(dllexport) void* GetMeshRendererChunk(Engine::ECS::Registry* registry, uint32_t chunkIndex) {
        return registry->GetStorage<Engine::ECS::MeshRenderer>().GetChunkPtr(chunkIndex);
    }

    __declspec(dllexport) uint32_t GetEntityCount(Engine::ECS::Registry* registry) {
        return (uint32_t)registry->GetStorage<Engine::ECS::Transform>().GetEntities().size();
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
    void(*updateDelegate)() = nullptr;
    void(*shutdownDelegate)() = nullptr;

    if (scriptHost.Initialize()) {
        auto initDelegate = (void(*)(void*, void*))scriptHost.GetMethodDelegate(
            L"ONEngine.Scripting.EngineHost, ONEngine.Scripting",
            L"Initialize",
            L"");
        
        if (initDelegate) initDelegate((void*)LogFromRuntime, &registry);

        updateDelegate = (void(*)())scriptHost.GetMethodDelegate(
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

    // 1. アセットのロード (テクスチャはマテリアルが自動ロードする)
    int32_t gridMatIdx = materialManager.LoadMaterial("Assets/Materials/Grid.mat");
    int32_t whiteMatIdx = materialManager.LoadMaterial("Assets/Materials/White.mat");

    shaderManager.LoadPipelineAsset("Assets/Pipelines/BindlessTest.json");
    shaderManager.LoadPipelineAsset("Assets/Pipelines/Blit.json");
    int32_t cubeModelIdx = assetManager.LoadModel("Packages/Models/primitive/cube.obj");

    // 2. エンティティの作成
    for (int i = 0; i < 20; ++i) {
        auto entity = registry.CreateEntity();
        auto& transform = registry.AddComponent<Transform>(entity);
        float angle = (i / 20.0f) * 3.141592f * 2.0f;
        transform.position = { cos(angle) * 15.0f, 0.0f, sin(angle) * 15.0f };
        transform.rotation = { 0.0f, -angle, 0.0f };
        transform.scale = { 0.5f, 0.5f, 0.5f };
        
        auto& meshRenderer = registry.AddComponent<MeshRenderer>(entity);
        meshRenderer.modelIndex = cubeModelIdx;
        meshRenderer.materialIndex = (i % 2 == 0) ? gridMatIdx : whiteMatIdx;
    }

    // 3. システムと共通定数バッファ
    Engine::ECS::RenderSystem renderSystem;
    Engine::Graphics::ConstantBuffer sceneCB;
    sceneCB.Create(graphicsEngine.GetRenderDevice(), sizeof(SceneData));

    Engine::Core::Timer timer;
    timer.Reset();

    // メインループ
    while(true) {
        window.Update();
        if(window.GetIsProcessEnd()) break;

        timer.Tick();
        float dt = timer.GetDeltaTime();

        if (updateDelegate) ((void(*)(float))updateDelegate)(dt);

        // 更新フェーズ
        /*
        registry.GetView<Transform>().Each([dt](auto entity, auto& transform) {
            transform.rotation.y += 1.0f * dt; // 1 radian per second
        });
        */

        // 描画準備
        renderer.ClearQueue();
        renderSystem.Update(registry);

        // 描画実行
        graphicsEngine.BeginFrame();
        
        // カメラ更新 (BeginFrameの後に行うことで、正しいフレームのリソースに書き込む)
        SceneData sceneData;
        auto view = Engine::Math::Matrix4x4::MakeLookAtLH({ 0, 20, -50 }, { 0, 0, 0 }, { 0, 1, 0 });
        auto proj = Engine::Math::Matrix4x4::MakePerspectiveFovLH(0.45f, 16.0f/9.0f, 0.1f, 1000.0f);
        sceneData.viewProj = view * proj;
        
        auto* currentFrameRes = graphicsEngine.GetCurrentFrameResource();
        currentFrameRes->GetSceneCB()->Update(&sceneData, sizeof(sceneData));

        renderer.Extract();
        
        graphicsEngine.Clear({ 0.7f, 0.7f, 0.7f, 1.0f });
        graphicsEngine.ClearDepth();
        
        // 描画コンテキストの構築
        Engine::Graphics::RenderContext context;
        context.commandList = graphicsEngine.GetCommandQueue()->GetCommandList();
        context.sceneCBAddress = currentFrameRes->GetSceneCB()->GetGPUVirtualAddress();
        context.frameIndex = graphicsEngine.GetCurrentFrameIndex();
        
        // Rendererに全てを任せる
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
