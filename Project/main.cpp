#include <Windows.h>
#include <vector>
#include "Engine/Core/Window.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/Resource/TextureManager.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Engine/Graphics/Resource/ModelLoader.h"
#include "Engine/Graphics/Resource/Mesh.h"
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
    window.Initialize(L"BindlessProperMeshTest", Engine::Math::Vector2Int::HD);

    auto& graphicsEngine = Engine::Graphics::GraphicsEngine::GetInstance();
    graphicsEngine.Initialize(window.GetHWND(), Engine::Math::Vector2Int::HD);

    auto& shaderManager = Engine::Graphics::ShaderManager::GetInstance();
    shaderManager.Initialize(graphicsEngine.GetRenderDevice());
    
    auto& textureManager = Engine::Graphics::TextureManager::GetInstance();
    textureManager.Initialize(graphicsEngine.GetRenderDevice());

    // 1. モデルとテクスチャをロード
    int32_t texIndex = textureManager.LoadTexture("TestTex", L"Packages/Textures/uvChecker.png");
    auto meshes = Engine::Graphics::ModelLoader::LoadModel(graphicsEngine.GetRenderDevice(), "Packages/Models/primitive/cube.obj");
    
    if (meshes.empty()) {
        Engine::Console::LogError("Failed to load model.");
        return -1;
    }

    // 2. パイプライン設定
    shaderManager.LoadPipelineAsset("Assets/Pipelines/BindlessTest.json");
    Engine::Graphics::PipelineStateDesc psoDesc;
    psoDesc.depthEnable = true;
    psoDesc.cullMode = D3D12_CULL_MODE_BACK;
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

        // カメラ行列の設定
        Engine::Math::Vector3 eye = { 0, 5, -15 };
        Engine::Math::Vector3 target = { 0, 0, 0 };
        Engine::Math::Vector3 up = { 0, 1, 0 };
        auto view = Engine::Math::Matrix4x4::MakeLookAtLH(eye, target, up);
        auto proj = Engine::Math::Matrix4x4::MakePerspectiveFovLH(0.45f, 16.0f/9.0f, 0.1f, 1000.0f);
        
        SceneData sceneData;
        sceneData.viewProj = view * proj;
        sceneCB.Update(&sceneData, sizeof(sceneData));

        graphicsEngine.BeginFrame();
        graphicsEngine.Clear({ 0.1f, 0.1f, 0.2f, 1.0f });
        graphicsEngine.ClearDepth();
        
        auto* commandList = graphicsEngine.GetCommandQueue()->GetCommandList();
        commandList->SetGraphicsRootSignature(rootSig->Get());
        commandList->SetPipelineState(pso->Get());

        // デスクリプタヒープのセット
        ID3D12DescriptorHeap* heaps[] = { textureManager.GetSrvHeap()->GetHeap() };
        commandList->SetDescriptorHeaps(_countof(heaps), heaps);

        // ルートパラメータの取得
        uint32_t sceneIdx = rootSig->GetParameterIndex("gSceneData");
        uint32_t matIdx = rootSig->GetParameterIndex("gMaterial");
        uint32_t texIdxRoot = rootSig->GetParameterIndex("gTextures");
        uint32_t vertIdxRoot = rootSig->GetParameterIndex("gVertices");

        // バインド
        commandList->SetGraphicsRootConstantBufferView(sceneIdx, sceneCB.GetGPUVirtualAddress());
        commandList->SetGraphicsRootConstantBufferView(matIdx, materialCB.GetGPUVirtualAddress());
        commandList->SetGraphicsRootDescriptorTable(texIdxRoot, textureManager.GetSrvHeap()->GetGPUHandle(0));
        
        // プリミティブトポロジーをセット
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // メッシュの描画
        for (const auto& mesh : meshes) {
            // 頂点バッファを Root SRV としてセット
            commandList->SetGraphicsRootShaderResourceView(vertIdxRoot, mesh->GetVertexBuffer()->GetResource()->GetGPUVirtualAddress());
            
            // インデックスバッファをセットして描画
            mesh->Draw(commandList);
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
