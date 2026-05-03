#include <Windows.h>
#include <vector>
#include <map>
#include "Engine/Core/Window.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/Resource/TextureManager.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Engine/Graphics/Resource/ModelLoader.h"
#include "Engine/Graphics/Resource/Mesh.h"
#include "Engine/ECS/Registry.h"
#include "Engine/ECS/Components/Transform.h"
#include "Engine/ECS/Components/MeshRenderer.h"
#include "Engine/Common/Console.h"

struct SceneData {
    Engine::Math::Matrix4x4 viewProj;
};

struct InstanceData {
    Engine::Math::Matrix4x4 world;
    uint32_t textureIndex;
    uint32_t _pad[3];
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Engine::Console::Initialize();

    Engine::Core::Window window;
    window.Initialize(L"BindlessInstancingECSTest", Engine::Math::Vector2Int::HD);

    auto& graphicsEngine = Engine::Graphics::GraphicsEngine::GetInstance();
    graphicsEngine.Initialize(window.GetHWND(), Engine::Math::Vector2Int::HD);

    auto& shaderManager = Engine::Graphics::ShaderManager::GetInstance();
    shaderManager.Initialize(graphicsEngine.GetRenderDevice());
    
    auto& textureManager = Engine::Graphics::TextureManager::GetInstance();
    textureManager.Initialize(graphicsEngine.GetRenderDevice());

    // 1. レジストリとアセットの準備
    Engine::ECS::Registry registry;
    
    int32_t gridTex = textureManager.LoadTexture("Grid", L"Packages/Textures/uvChecker.png");
    int32_t whiteTex = textureManager.LoadTexture("White", L"Packages/Textures/white.png");
    
    std::string modelPath = "Packages/Models/primitive/cube.obj";
    auto sharedMeshes = Engine::Graphics::ModelLoader::LoadModel(graphicsEngine.GetRenderDevice(), modelPath);

    if (sharedMeshes.empty()) {
        Engine::Console::LogError("Failed to load model.");
        return -1;
    }

    // 大量のエンティティを作成 (インスタンシングのテスト)
    const int kNumEntities = 10;
    for (int i = 0; i < kNumEntities; ++i) {
        auto entity = registry.CreateEntity();
        auto& transform = registry.AddComponent<Engine::ECS::Transform>(entity);
        transform.position = { (i - (kNumEntities/2.0f)) * 4.0f, 0.0f, 0.0f };
        transform.rotation = { 0.0f, i * 0.2f, 0.0f };
        transform.scale = { 0.4f, 0.4f, 0.4f };
        
        auto& renderer = registry.AddComponent<Engine::ECS::MeshRenderer>(entity);
        renderer.meshPath = modelPath;
        renderer.textureIndex = (i % 2 == 0) ? gridTex : whiteTex;
    }

    // 2. パイプライン設定
    shaderManager.LoadPipelineAsset("Assets/Pipelines/BindlessTest.json");
    Engine::Graphics::PipelineStateDesc psoDesc;
    psoDesc.depthEnable = true;
    auto* pso = shaderManager.GetOrCreatePSO("BindlessTest", psoDesc);
    auto* rootSig = shaderManager.GetRootSignature("BindlessTest");
    
    // 3. バッファ作成
    Engine::Graphics::ConstantBuffer sceneCB;
    sceneCB.Create(graphicsEngine.GetRenderDevice(), sizeof(SceneData));

    // 全エンティティ分のインスタンスデータバッファ (StructuredBuffer)
    Engine::Graphics::StructuredBuffer instanceSB;
    instanceSB.Create(graphicsEngine.GetRenderDevice(), sizeof(InstanceData), kNumEntities);

    // メインループ
    while(true) {
        window.Update();
        if(window.GetIsProcessEnd()) break;

        // --- Extract Phase (データ抽出フェーズ) ---
        std::vector<InstanceData> extractedData;
        registry.GetView<Engine::ECS::Transform, Engine::ECS::MeshRenderer>().Each([&](auto entity, auto& transform, auto& renderer) {
            InstanceData data;
            data.world = Engine::Math::Matrix4x4::MakeAffine(transform.scale, transform.rotation, transform.position);
            data.textureIndex = renderer.textureIndex;
            extractedData.push_back(data);
        });

        // GPUバッファの更新
        if (!extractedData.empty()) {
            instanceSB.Update(extractedData.data(), static_cast<uint32_t>(extractedData.size() * sizeof(InstanceData)));
        }

        // カメラ更新
        Engine::Math::Vector3 eye = { 0, 10, -40 };
        Engine::Math::Vector3 target = { 0, 0, 0 };
        Engine::Math::Vector3 up = { 0, 1, 0 };
        auto view = Engine::Math::Matrix4x4::MakeLookAtLH(eye, target, up);
        auto proj = Engine::Math::Matrix4x4::MakePerspectiveFovLH(0.45f, 16.0f/9.0f, 0.1f, 1000.0f);
        
        SceneData sceneData;
        sceneData.viewProj = view * proj;
        sceneCB.Update(&sceneData, sizeof(sceneData));

        // --- Draw Phase (描画フェーズ) ---
        graphicsEngine.BeginFrame();
        graphicsEngine.Clear({ 0.1f, 0.15f, 0.2f, 1.0f });
        graphicsEngine.ClearDepth();
        
        auto* commandList = graphicsEngine.GetCommandQueue()->GetCommandList();
        commandList->SetGraphicsRootSignature(rootSig->Get());
        commandList->SetPipelineState(pso->Get());

        ID3D12DescriptorHeap* heaps[] = { textureManager.GetSrvHeap()->GetHeap() };
        commandList->SetDescriptorHeaps(_countof(heaps), heaps);

        uint32_t sceneIdx = rootSig->GetParameterIndex("gSceneData");
        uint32_t instIdx = rootSig->GetParameterIndex("gInstances");
        uint32_t texIdxRoot = rootSig->GetParameterIndex("gTextures");
        uint32_t vertIdxRoot = rootSig->GetParameterIndex("gVertices");

        commandList->SetGraphicsRootConstantBufferView(sceneIdx, sceneCB.GetGPUVirtualAddress());
        commandList->SetGraphicsRootDescriptorTable(texIdxRoot, textureManager.GetSrvHeap()->GetGPUHandle(0));
        commandList->SetGraphicsRootShaderResourceView(instIdx, instanceSB.GetResource()->GetGPUVirtualAddress());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // メッシュごとにインスタンシング描画
        // 今回は全てのエンティティが同じメッシュ (sharedMeshes) を使っている前提
        uint32_t instanceCount = static_cast<uint32_t>(extractedData.size());
        for (const auto& mesh : sharedMeshes) {
            commandList->SetGraphicsRootShaderResourceView(vertIdxRoot, mesh->GetVertexBuffer()->GetResource()->GetGPUVirtualAddress());
            
            // mesh->Draw を使わず、直接インスタンシング描画を呼ぶ
            D3D12_INDEX_BUFFER_VIEW ibv = mesh->GetIndexBuffer()->GetView();
            commandList->IASetIndexBuffer(&ibv);
            commandList->DrawIndexedInstanced(mesh->GetIndexBuffer()->GetCount(), instanceCount, 0, 0, 0);
        }

        graphicsEngine.EndFrame();
    }

    textureManager.Shutdown();
    shaderManager.Shutdown();
    graphicsEngine.Shutdown();
    window.Shutdown();
    Engine::Console::Shutdown();

    return 0;
}
