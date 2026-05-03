#include <Windows.h>
#include <vector>
#include "Engine/Core/Window.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Core/Renderer.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/Resource/TextureManager.h"
#include "Engine/Graphics/Resource/MaterialManager.h"
#include "Engine/Graphics/Resource/AssetManager.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Engine/ECS/Registry.h"
#include "Engine/ECS/Components/Transform.h"
#include "Engine/ECS/Components/MeshRenderer.h"
#include "Engine/ECS/Systems/RenderSystem.h"
#include "Engine/Common/Console.h"
#include "Schema/Schema.h"

using namespace Engine::GeneratedSchema;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Engine::Console::Initialize();

    Engine::Core::Window window;
    window.Initialize(L"FinalRenderSystemTest", Engine::Math::Vector2Int::HD);

    auto& graphicsEngine = Engine::Graphics::GraphicsEngine::GetInstance();
    graphicsEngine.Initialize(window.GetHWND(), Engine::Math::Vector2Int::HD);

    auto& shaderManager = Engine::Graphics::ShaderManager::GetInstance();
    shaderManager.Initialize(graphicsEngine.GetRenderDevice());
    
    auto& textureManager = Engine::Graphics::TextureManager::GetInstance();
    textureManager.Initialize(graphicsEngine.GetRenderDevice());

    auto& materialManager = Engine::Graphics::MaterialManager::GetInstance();
    materialManager.Initialize(graphicsEngine.GetRenderDevice());

    auto& assetManager = Engine::AssetManager::GetInstance();
    assetManager.Initialize(graphicsEngine.GetRenderDevice());

    auto& renderer = Engine::Graphics::Renderer::GetInstance();
    renderer.Initialize(graphicsEngine.GetRenderDevice());

    // 1. アセットのロード
    textureManager.LoadTexture("Grid", L"Packages/Textures/uvChecker.png");
    textureManager.LoadTexture("White", L"Packages/Textures/white.png");

    materialManager.LoadMaterial("Assets/Materials/Grid.mat");
    materialManager.LoadMaterial("Assets/Materials/White.mat");

    shaderManager.LoadPipelineAsset("Assets/Pipelines/BindlessTest.json");
    
    std::string modelName = "Cube";
    assetManager.LoadModel(modelName, "Packages/Models/primitive/cube.obj");

    // 2. エンティティの作成
    Engine::ECS::Registry registry;
    for (int i = 0; i < 20; ++i) {
        auto entity = registry.CreateEntity();
        auto& transform = registry.AddComponent<Engine::ECS::Transform>(entity);
        // 円状に配置
        float angle = (i / 20.0f) * 3.141592f * 2.0f;
        transform.position = { cos(angle) * 15.0f, 0.0f, sin(angle) * 15.0f };
        transform.rotation = { 0.0f, -angle, 0.0f };
        transform.scale = { 0.5f, 0.5f, 0.5f };
        
        auto& meshRenderer = registry.AddComponent<Engine::ECS::MeshRenderer>(entity);
        meshRenderer.modelName = modelName;
        meshRenderer.materialName = (i % 2 == 0) ? "GridMat" : "WhiteMat";
    }

    // 3. システムと共通バッファ
    Engine::ECS::RenderSystem renderSystem;

    Engine::Graphics::ConstantBuffer sceneCB;
    sceneCB.Create(graphicsEngine.GetRenderDevice(), sizeof(SceneData));

    // メインループ
    while(true) {
        window.Update();
        if(window.GetIsProcessEnd()) break;

        // --- Update Phase ---
        // エンティティを少し回転させる（ECSの動的更新テスト）
        registry.GetView<Engine::ECS::Transform>().Each([](auto entity, auto& transform) {
            transform.rotation.y += 0.01f;
        });

        // --- Render Phase ---
        
        // 1. カメラ更新
        SceneData sceneData;
        auto view = Engine::Math::Matrix4x4::MakeLookAtLH({ 0, 20, -50 }, { 0, 0, 0 }, { 0, 1, 0 });
        auto proj = Engine::Math::Matrix4x4::MakePerspectiveFovLH(0.45f, 16.0f/9.0f, 0.1f, 1000.0f);
        sceneData.viewProj = view * proj;
        sceneCB.Update(&sceneData, sizeof(sceneData));

        // 2. 描画リクエストの収集
        renderer.ClearQueue();
        renderSystem.Update(registry);

        // 3. 実際の描画
        graphicsEngine.BeginFrame();
        graphicsEngine.Clear({ 0.05f, 0.05f, 0.08f, 1.0f });
        graphicsEngine.ClearDepth();
        
        auto* commandList = graphicsEngine.GetCommandQueue()->GetCommandList();
        renderer.Render(commandList, sceneCB.GetGPUVirtualAddress());

        graphicsEngine.EndFrame();
    }

    renderer.Shutdown();
    assetManager.Shutdown();
    materialManager.Shutdown();
    textureManager.Shutdown();
    shaderManager.Shutdown();
    graphicsEngine.Shutdown();
    window.Shutdown();
    Engine::Console::Shutdown();

    return 0;
}
