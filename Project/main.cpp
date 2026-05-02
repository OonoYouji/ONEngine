#include <Windows.h>
#include <vector>
#include <cmath>
#include "Engine/Core/Window.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/Resource/Mesh.h"
#include "Engine/Graphics/Resource/ModelLoader.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Engine/Common/Console.h"

struct SceneData {
    Engine::Math::Matrix4x4 viewProj;
};

struct ObjectData {
    Engine::Math::Matrix4x4 world;
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Engine::Console::Initialize();

    Engine::Core::Window window;
    window.Initialize(L"ModelLoaderTest", Engine::Math::Vector2Int::HD);

    auto& graphicsEngine = Engine::Graphics::GraphicsEngine::GetInstance();
    graphicsEngine.Initialize(window.GetHWND(), Engine::Math::Vector2Int::HD);

    auto& shaderManager = Engine::Graphics::ShaderManager::GetInstance();
    shaderManager.Initialize(graphicsEngine.GetRenderDevice());

    // 1. テンプレートロード
    shaderManager.LoadPipelineAsset("Assets/Pipelines/TestTemplate.json");
    Engine::Graphics::PipelineStateDesc psoDesc;
    psoDesc.depthEnable = true;
    psoDesc.dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psoDesc.cullMode = D3D12_CULL_MODE_BACK;

    auto* pso = shaderManager.GetOrCreatePSO("TestTemplate", psoDesc);
    auto* rootSig = shaderManager.GetRootSignature("TestTemplate");

    // 2. モデルロード (GLTFテスト: walk)
    auto meshes = Engine::Graphics::ModelLoader::LoadModel(graphicsEngine.GetRenderDevice(), "Packages/Models/Human/walk.gltf");


    if (meshes.empty()) {
        Engine::Console::LogError("Model failed to load or has no meshes.");
    }

    // 3. 定数バッファ作成
    Engine::Graphics::ConstantBuffer sceneCB;
    sceneCB.Create(graphicsEngine.GetRenderDevice(), sizeof(SceneData));

    // 4. デスクリプタの作成 (SRVHeapを使用)
    auto* device = graphicsEngine.GetRenderDevice()->GetDevice();
    auto* srvHeap = graphicsEngine.GetSRVHeap();

    const uint32_t kIdxScene = 0;
    const uint32_t kIdxMesh  = 1;

    // Viewの作成
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = sceneCB.GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = 256;
    device->CreateConstantBufferView(&cbvDesc, srvHeap->GetCPUHandle(kIdxScene));

    float angle = 0.0f;
    while(true) {
        window.Update();
        if(window.GetIsProcessEnd()) break;

        angle += 0.01f;

        // --- カメラ行列の計算 ---
        Engine::Math::Matrix4x4 view = Engine::Math::Matrix4x4::MakeLookAtLH(
            { 0.0f, 2.0f, -5.0f }, // Eye (少し上から)
            { 0.0f, 0.0f, 0.0f },  // Target
            { 0.0f, 1.0f, 0.0f }   // Up
        );
        Engine::Math::Matrix4x4 proj = Engine::Math::Matrix4x4::MakePerspectiveFovLH(
            0.8f,
            (float)Engine::Math::Vector2Int::HD.x / Engine::Math::Vector2Int::HD.y,
            0.1f, 100.0f
        );
        
        graphicsEngine.BeginFrame();
        graphicsEngine.Clear({ 0.1f, 0.1f, 0.1f, 1.0f });
        graphicsEngine.ClearDepth();

        auto* commandList = graphicsEngine.GetCommandQueue()->GetCommandList();
        commandList->SetGraphicsRootSignature(rootSig->Get());
        commandList->SetPipelineState(pso->Get());

        ID3D12DescriptorHeap* heaps[] = { srvHeap->GetHeap() };
        commandList->SetDescriptorHeaps(_countof(heaps), heaps);

        uint32_t sceneIdx = rootSig->GetParameterIndex("gSceneData");
        uint32_t meshIdx  = rootSig->GetParameterIndex("gVertices");

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 各メッシュを描画
        for (const auto& mesh : meshes) {
            // World * ViewProj
            Engine::Math::Matrix4x4 world = Engine::Math::Matrix4x4::MakeRotateY(angle);
            SceneData drawData;
            drawData.viewProj = world * view * proj;
            sceneCB.Update(&drawData, sizeof(SceneData));

            // Mesh SRV をヒープの特定の場所に動的に作る（今回はテストなので固定1スロットを使い回す）
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = DXGI_FORMAT_UNKNOWN;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Buffer.NumElements = mesh->GetVertexBuffer()->GetCount();
            srvDesc.Buffer.StructureByteStride = mesh->GetVertexBuffer()->GetStride();
            device->CreateShaderResourceView(mesh->GetVertexBuffer()->GetResource(), &srvDesc, srvHeap->GetCPUHandle(kIdxMesh));

            commandList->SetGraphicsRootDescriptorTable(sceneIdx, srvHeap->GetGPUHandle(kIdxScene));
            commandList->SetGraphicsRootDescriptorTable(meshIdx, srvHeap->GetGPUHandle(kIdxMesh));
            
            mesh->Draw(commandList);
        }

        graphicsEngine.EndFrame();
    }

    shaderManager.Shutdown();
    graphicsEngine.Shutdown();
    window.Shutdown();
    Engine::Console::Shutdown();

    return 0;
}
