#include <Windows.h>
#include <vector>
#include "Engine/Core/Window.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/Resource/Mesh.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Engine/Common/Console.h"

struct SceneData {
    Engine::Math::Matrix4x4 viewProj;
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Engine::Console::Initialize();

    Engine::Core::Window window;
    window.Initialize(L"MeshDrawReflectionTest", Engine::Math::Vector2Int::HD);

    auto& graphicsEngine = Engine::Graphics::GraphicsEngine::GetInstance();
    graphicsEngine.Initialize(window.GetHWND(), Engine::Math::Vector2Int::HD);

    auto& shaderManager = Engine::Graphics::ShaderManager::GetInstance();
    shaderManager.Initialize(graphicsEngine.GetRenderDevice());

    // 1. テンプレートロード (パスを修正)
    if (!shaderManager.LoadPipelineAsset("Assets/Pipelines/TestTemplate.json")) {
        Engine::Console::LogError("Failed to load Pipeline Asset.");
    }

    Engine::Graphics::PipelineStateDesc psoDesc;
    psoDesc.depthEnable = false;
    psoDesc.dsvFormat = DXGI_FORMAT_UNKNOWN;

    auto* pso = shaderManager.GetOrCreatePSO("TestTemplate", psoDesc);
    auto* rootSig = shaderManager.GetRootSignature("TestTemplate");

    if (!pso || !rootSig) {
        Engine::Console::LogError("PSO or RootSignature is null. Check shader compilation logs.");
        // 以降の処理をスキップするか、適切に終了させる
    } else {
        // 2. メッシュ作成 (三角形)
        std::vector<Engine::Graphics::Vertex> vertices = {
            {{ 0.0f,  0.5f, 0.0f}, {0.5f, 0.0f}},
            {{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
        };
        std::vector<uint32_t> indices = { 0, 1, 2 };
        Engine::Graphics::Mesh mesh;
        mesh.Create(graphicsEngine.GetRenderDevice(), vertices, indices);

        // 3. 定数バッファ作成
        Engine::Graphics::ConstantBuffer constantBuffer;
        constantBuffer.Create(graphicsEngine.GetRenderDevice(), sizeof(SceneData));

        SceneData sceneData;
        sceneData.viewProj = Engine::Math::Matrix4x4::kIdentity;
        constantBuffer.Update(&sceneData, sizeof(SceneData));

        // 4. ビュー（デスクリプタ）の作成
        auto* device = graphicsEngine.GetRenderDevice()->GetDevice();
        auto* srvHeap = graphicsEngine.GetSRVHeap();

        // Reflectionを活用してパラメータ番号を取得
        uint32_t sceneDataParamIndex = rootSig->GetParameterIndex("gSceneData");
        uint32_t verticesParamIndex = rootSig->GetParameterIndex("gVertices");

        // CBV (gSceneData)
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = constantBuffer.GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = (sizeof(SceneData) + 255) & ~255;
        device->CreateConstantBufferView(&cbvDesc, srvHeap->GetCPUHandle(0));

        // SRV (gVertices)
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = static_cast<uint32_t>(vertices.size());
        srvDesc.Buffer.StructureByteStride = sizeof(Engine::Graphics::Vertex);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        device->CreateShaderResourceView(mesh.GetVertexBuffer()->GetResource(), &srvDesc, srvHeap->GetCPUHandle(1));

        while(true) {
            window.Update();
            if(window.GetIsProcessEnd()) break;

            graphicsEngine.BeginFrame();
            graphicsEngine.Clear({ 0.1f, 0.2f, 0.3f, 1.0f });

            auto* commandList = graphicsEngine.GetCommandQueue()->GetCommandList();
            
            commandList->SetGraphicsRootSignature(rootSig->Get());
            commandList->SetPipelineState(pso->Get());

            ID3D12DescriptorHeap* heaps[] = { srvHeap->GetHeap() };
            commandList->SetDescriptorHeaps(_countof(heaps), heaps);

            commandList->SetGraphicsRootDescriptorTable(sceneDataParamIndex, srvHeap->GetGPUHandle(0));
            commandList->SetGraphicsRootDescriptorTable(verticesParamIndex, srvHeap->GetGPUHandle(1));

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            mesh.Draw(commandList);

            graphicsEngine.EndFrame();
        }
    }

    shaderManager.Shutdown();
    graphicsEngine.Shutdown();
    window.Shutdown();
    Engine::Console::Shutdown();

    return 0;
}
