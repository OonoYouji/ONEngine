#include <Windows.h>
#include <vector>
#include <cmath>
#include "Engine/Core/Window.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/Resource/Mesh.h"
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
    window.Initialize(L"DepthCollisionTest", Engine::Math::Vector2Int::HD);

    auto& graphicsEngine = Engine::Graphics::GraphicsEngine::GetInstance();
    graphicsEngine.Initialize(window.GetHWND(), Engine::Math::Vector2Int::HD);

    auto& shaderManager = Engine::Graphics::ShaderManager::GetInstance();
    shaderManager.Initialize(graphicsEngine.GetRenderDevice());

    // 1. テンプレートロード
    shaderManager.LoadPipelineAsset("Assets/Pipelines/TestTemplate.json");
    Engine::Graphics::PipelineStateDesc psoDesc;
    psoDesc.depthEnable = true;
    psoDesc.dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psoDesc.cullMode = D3D12_CULL_MODE_NONE; // 交差を見やすくするため両面描画

    auto* pso = shaderManager.GetOrCreatePSO("TestTemplate", psoDesc);
    auto* rootSig = shaderManager.GetRootSignature("TestTemplate");

    // 2. メッシュ作成 (少し大きめの三角形)
    std::vector<Engine::Graphics::Vertex> vertices = {
        {{ 0.0f,  1.0f, 0.0f}, {0.5f, 0.0f}},
        {{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
    };
    std::vector<uint32_t> indices = { 0, 1, 2 };
    Engine::Graphics::Mesh mesh;
    mesh.Create(graphicsEngine.GetRenderDevice(), vertices, indices);

    // 3. 定数バッファ作成 (Scene用とObject用)
    Engine::Graphics::ConstantBuffer sceneCB;
    sceneCB.Create(graphicsEngine.GetRenderDevice(), sizeof(SceneData));

    Engine::Graphics::ConstantBuffer objectCB1;
    objectCB1.Create(graphicsEngine.GetRenderDevice(), sizeof(ObjectData));

    Engine::Graphics::ConstantBuffer objectCB2;
    objectCB2.Create(graphicsEngine.GetRenderDevice(), sizeof(ObjectData));

    // 4. デスクリプタの作成 (SRVHeapを使用)
    auto* device = graphicsEngine.GetRenderDevice()->GetDevice();
    auto* srvHeap = graphicsEngine.GetSRVHeap();

    // インデックス定義
    const uint32_t kIdxScene = 0;
    const uint32_t kIdxMesh  = 1;
    const uint32_t kIdxObj1  = 2;
    const uint32_t kIdxObj2  = 3;

    // Viewの作成
    auto createCBV = [&](Engine::Graphics::ConstantBuffer& cb, uint32_t heapIdx) {
        D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
        desc.BufferLocation = cb.GetGPUVirtualAddress();
        desc.SizeInBytes = 256;
        device->CreateConstantBufferView(&desc, srvHeap->GetCPUHandle(heapIdx));
    };

    createCBV(sceneCB, kIdxScene);
    createCBV(objectCB1, kIdxObj1);
    createCBV(objectCB2, kIdxObj2);

    // Mesh SRV
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.NumElements = static_cast<uint32_t>(vertices.size());
    srvDesc.Buffer.StructureByteStride = sizeof(Engine::Graphics::Vertex);
    device->CreateShaderResourceView(mesh.GetVertexBuffer()->GetResource(), &srvDesc, srvHeap->GetCPUHandle(kIdxMesh));

    float angle = 0.0f;
    while(true) {
        window.Update();
        if(window.GetIsProcessEnd()) break;

        angle += 0.01f;

        // --- カメラ行列の計算 ---
        Engine::Math::Matrix4x4 view = Engine::Math::Matrix4x4::MakeLookAtLH(
            { 0.0f, 0.0f, -5.0f }, // Eye
            { 0.0f, 0.0f, 0.0f },  // Target
            { 0.0f, 1.0f, 0.0f }   // Up
        );
        Engine::Math::Matrix4x4 proj = Engine::Math::Matrix4x4::MakePerspectiveFovLH(
            0.8f, // 45度くらい
            (float)Engine::Math::Vector2Int::HD.x / Engine::Math::Vector2Int::HD.y,
            0.1f, 100.0f
        );
        SceneData sceneData;
        sceneData.viewProj = view * proj;
        sceneCB.Update(&sceneData, sizeof(SceneData));

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
        // ※ObjectData用のレジストリがHLSLにないので、今回はgSceneDataを再利用するか、HLSLを修正する必要があります。
        // テストを簡略化するため、gSceneData に World * ViewProj を流し込む形にします。

        commandList->SetGraphicsRootDescriptorTable(meshIdx, srvHeap->GetGPUHandle(kIdxMesh));
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 1つ目の三角形 (少し前後に傾ける)
        ObjectData obj1;
        obj1.world = Engine::Math::Matrix4x4::MakeRotateY(angle);
        SceneData drawData1;
        drawData1.viewProj = obj1.world * sceneData.viewProj;
        sceneCB.Update(&drawData1, sizeof(SceneData));
        commandList->SetGraphicsRootDescriptorTable(sceneIdx, srvHeap->GetGPUHandle(kIdxScene));
        mesh.Draw(commandList);

        // 2つ目の三角形 (交差するように逆回転・配置)
        ObjectData obj2;
        obj2.world = Engine::Math::Matrix4x4::MakeRotateX(angle * 0.5f);
        SceneData drawData2;
        drawData2.viewProj = obj2.world * sceneData.viewProj;
        objectCB1.Update(&drawData2, sizeof(SceneData)); // バッファを分けておく
        commandList->SetGraphicsRootDescriptorTable(sceneIdx, srvHeap->GetGPUHandle(kIdxObj1));
        mesh.Draw(commandList);

        graphicsEngine.EndFrame();
    }

    shaderManager.Shutdown();
    graphicsEngine.Shutdown();
    window.Shutdown();
    Engine::Console::Shutdown();

    return 0;
}
