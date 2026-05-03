#include <Windows.h>
#include <vector>
#include "Engine/Core/Window.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/Resource/TextureManager.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Engine/Common/Console.h"

struct SceneData {
    Engine::Math::Matrix4x4 viewProj;
};

struct MaterialData {
    uint32_t textureIndex;
    float _pad[3];
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Engine::Console::Initialize();

    Engine::Core::Window window;
    window.Initialize(L"BindlessTest", Engine::Math::Vector2Int::HD);

    auto& graphicsEngine = Engine::Graphics::GraphicsEngine::GetInstance();
    graphicsEngine.Initialize(window.GetHWND(), Engine::Math::Vector2Int::HD);

    auto& shaderManager = Engine::Graphics::ShaderManager::GetInstance();
    shaderManager.Initialize(graphicsEngine.GetRenderDevice());
    
    auto& textureManager = Engine::Graphics::TextureManager::GetInstance();
    textureManager.Initialize(graphicsEngine.GetRenderDevice());

    // 1. テクスチャをロード
    int32_t texIndex = textureManager.LoadTexture("TestTex", L"Packages/Textures/uvChecker.png");
    if (texIndex < 0) {
        Engine::Console::LogError("Failed to load test texture.");
    }

    // 2. パイプライン設定
    shaderManager.LoadPipelineAsset("Assets/Pipelines/BindlessTest.json"); // 新しいパイプライン定義
    Engine::Graphics::PipelineStateDesc psoDesc;
    psoDesc.depthEnable = false; // 2Dなので深度不要
    psoDesc.dsvFormat = DXGI_FORMAT_UNKNOWN;
    auto* pso = shaderManager.GetOrCreatePSO("BindlessTest", psoDesc);
    auto* rootSig = shaderManager.GetRootSignature("BindlessTest");
    
    // 3. 定数バッファ作成
    Engine::Graphics::ConstantBuffer sceneCB;
    sceneCB.Create(graphicsEngine.GetRenderDevice(), sizeof(SceneData));

    Engine::Graphics::ConstantBuffer materialCB;
    materialCB.Create(graphicsEngine.GetRenderDevice(), sizeof(MaterialData));

    MaterialData matData = { static_cast<uint32_t>(texIndex) };
    materialCB.Update(&matData, sizeof(matData));

    // メインループ
    while(true) {
        window.Update();
        if(window.GetIsProcessEnd()) break;

        SceneData sceneData;
        sceneData.viewProj = Engine::Math::Matrix4x4::kIdentity; // 2Dなので単位行列
        sceneCB.Update(&sceneData, sizeof(sceneData));

        graphicsEngine.BeginFrame();
        graphicsEngine.Clear({ 0.1f, 0.1f, 0.1f, 1.0f });
        
        auto* commandList = graphicsEngine.GetCommandQueue()->GetCommandList();
        commandList->SetGraphicsRootSignature(rootSig->Get());
        commandList->SetPipelineState(pso->Get());

        ID3D12DescriptorHeap* heaps[] = { textureManager.GetSrvHeap()->GetHeap() };
        commandList->SetDescriptorHeaps(_countof(heaps), heaps);

        uint32_t sceneIdx = rootSig->GetParameterIndex("gSceneData");
        uint32_t matIdx = rootSig->GetParameterIndex("gMaterial");
        uint32_t texIdxRoot = rootSig->GetParameterIndex("gTextures");

        commandList->SetGraphicsRootConstantBufferView(sceneIdx, sceneCB.GetGPUVirtualAddress());
        commandList->SetGraphicsRootConstantBufferView(matIdx, materialCB.GetGPUVirtualAddress());
        commandList->SetGraphicsRootDescriptorTable(texIdxRoot, textureManager.GetSrvHeap()->GetGPUHandle(0));

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawInstanced(3, 1, 0, 0); // 頂点3つで三角形を描画

        graphicsEngine.EndFrame();
    }

    textureManager.Shutdown();
    shaderManager.Shutdown();
    graphicsEngine.Shutdown();
    window.Shutdown();
    Engine::Console::Shutdown();

    return 0;
}
