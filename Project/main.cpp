#include <Windows.h>
#include <vector>
#include "Engine/Core/Window.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/Resource/TextureManager.h"
#include "Engine/Graphics/Resource/MaterialManager.h"
#include "Engine/Graphics/Resource/AssetManager.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
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
    window.Initialize(L"AssetSystemTest", Engine::Math::Vector2Int::HD);

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
    for (int i = 0; i < 10; ++i) {
        auto entity = registry.CreateEntity();
        auto& transform = registry.AddComponent<Engine::ECS::Transform>(entity);
        transform.position = { (i - 5.0f) * 4.0f, 0.0f, 0.0f };
        transform.rotation = { 0.0f, i * 0.2f, 0.0f };
        transform.scale = { 0.4f, 0.4f, 0.4f };
        
        auto& renderer = registry.AddComponent<Engine::ECS::MeshRenderer>(entity);
        renderer.modelName = modelName;
        renderer.materialName = (i % 2 == 0) ? "GridMat" : "WhiteMat";
    }

    // 3. バッファ作成
    Engine::Graphics::ConstantBuffer sceneCB;
    sceneCB.Create(graphicsEngine.GetRenderDevice(), sizeof(SceneData));

    Engine::Graphics::StructuredBuffer instanceSB;
    instanceSB.Create(graphicsEngine.GetRenderDevice(), sizeof(InstanceData), 10);

    // メインループ
    while(true) {
        window.Update();
        if(window.GetIsProcessEnd()) break;

        // --- Extract Phase ---
        std::vector<InstanceData> extractedData;
        registry.GetView<Engine::ECS::Transform, Engine::ECS::MeshRenderer>().Each([&](auto entity, auto& transform, auto& renderer) {
            auto* mat = materialManager.GetMaterial(renderer.materialName);
            if (!mat) return;

            InstanceData data;
            data.world = Engine::Math::Matrix4x4::MakeAffine(transform.scale, transform.rotation, transform.position);
            data.textureIndex = textureManager.GetTexture(mat->textureName)->GetIndex();
            extractedData.push_back(data);
        });

        if (!extractedData.empty()) {
            instanceSB.Update(extractedData.data(), static_cast<uint32_t>(extractedData.size() * sizeof(InstanceData)));
        }

        // カメラ更新
        SceneData sceneData;
        auto view = Engine::Math::Matrix4x4::MakeLookAtLH({ 0, 10, -40 }, { 0, 0, 0 }, { 0, 1, 0 });
        auto proj = Engine::Math::Matrix4x4::MakePerspectiveFovLH(0.45f, 16.0f/9.0f, 0.1f, 1000.0f);
        sceneData.viewProj = view * proj;
        sceneCB.Update(&sceneData, sizeof(sceneData));

        // --- Draw Phase ---
        graphicsEngine.BeginFrame();
        graphicsEngine.Clear({ 0.1f, 0.12f, 0.15f, 1.0f });
        graphicsEngine.ClearDepth();
        
        auto* commandList = graphicsEngine.GetCommandQueue()->GetCommandList();
        
        // 簡易的な描画ループ（本来はパイプラインごとにグループ化すべき）
        registry.GetView<Engine::ECS::Transform, Engine::ECS::MeshRenderer>().Each([&](auto entity, auto& transform, auto& renderer) {
            auto* mat = materialManager.GetMaterial(renderer.materialName);
            if (!mat) return;

            auto* pso = shaderManager.GetOrCreatePSO(mat->pipelineName, {});
            auto* rootSig = shaderManager.GetRootSignature(mat->pipelineName);
            
            commandList->SetGraphicsRootSignature(rootSig->Get());
            commandList->SetPipelineState(pso->Get());

            ID3D12DescriptorHeap* heaps[] = { textureManager.GetSrvHeap()->GetHeap() };
            commandList->SetDescriptorHeaps(_countof(heaps), heaps);

            commandList->SetGraphicsRootConstantBufferView(rootSig->GetParameterIndex("gSceneData"), sceneCB.GetGPUVirtualAddress());
            commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gTextures"), textureManager.GetSrvHeap()->GetGPUHandle(0));
            commandList->SetGraphicsRootShaderResourceView(rootSig->GetParameterIndex("gInstances"), instanceSB.GetResource()->GetGPUVirtualAddress());

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            const auto& meshes = assetManager.GetMeshes(renderer.modelName);
            uint32_t instanceIdx = static_cast<uint32_t>(&transform - &registry.GetComponent<Engine::ECS::Transform>(registry.GetStorage<Engine::ECS::Transform>().GetEntities()[0]));
            // 注意：この簡易的なインデックス取得は、Sparse Setの構造に依存するため、本来はIDを保持すべき
            // 今回は簡略化のため、インスタンシングを1つずつ呼ぶ形（非効率だが動作確認用）に一旦戻すか、
            // もしくはインスタンスIDを別途渡す必要がある。
            
            // 修正：インスタンシングのメリットを活かすため、同じメッシュ・マテリアルをまとめて描画するのが理想。
            // ここでは「全エンティティが同じメッシュ・マテリアル構成」と仮定して、ループの外で一括描画する。
        });

        // インスタンシング描画（一括）
        uint32_t instanceCount = static_cast<uint32_t>(extractedData.size());
        if (instanceCount > 0) {
            auto* firstRenderer = &registry.GetComponent<Engine::ECS::MeshRenderer>(registry.GetStorage<Engine::ECS::MeshRenderer>().GetEntities()[0]);
            auto* mat = materialManager.GetMaterial(firstRenderer->materialName);
            auto* rootSig = shaderManager.GetRootSignature(mat->pipelineName);
            uint32_t vertIdxRoot = rootSig->GetParameterIndex("gVertices");
            
            const auto& meshes = assetManager.GetMeshes(firstRenderer->modelName);
            for (const auto& mesh : meshes) {
                commandList->SetGraphicsRootShaderResourceView(vertIdxRoot, mesh->GetVertexBuffer()->GetResource()->GetGPUVirtualAddress());
                D3D12_INDEX_BUFFER_VIEW ibv = mesh->GetIndexBuffer()->GetView();
                commandList->IASetIndexBuffer(&ibv);
                commandList->DrawIndexedInstanced(mesh->GetIndexBuffer()->GetCount(), instanceCount, 0, 0, 0);
            }
        }

        graphicsEngine.EndFrame();
    }

    assetManager.Shutdown();
    materialManager.Shutdown();
    textureManager.Shutdown();
    shaderManager.Shutdown();
    graphicsEngine.Shutdown();
    window.Shutdown();
    Engine::Console::Shutdown();

    return 0;
}
