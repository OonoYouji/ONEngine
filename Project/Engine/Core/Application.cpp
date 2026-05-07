#include "Application.h"
#include "Engine/Common/Console.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Core/Renderer.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Asset/MaterialManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/FontManager.h"
#include "Engine/Graphics/Resource/GeometryPool.h"
#include "Engine/Graphics/PostProcess/DebugRenderer.h"
#include "Engine/Scene/SceneLoader.h"
#include "Engine/Script/ScriptHost.h"
#include "Engine/ECS/ComponentRegistry.h"
#include "Engine/ECS/Systems/SpriteSystem.h"

extern "C" void LogFromRuntime(const char*);

namespace Engine::Core {

bool Application::Initialize(HINSTANCE hInstance, int nCmdShow) {
    Console::Initialize();

    // 1. 基本システムの初期化
    ECS::InitializeComponentRegistry();
    window_.Initialize(L"ONEngine", Math::Vector2Int::HD);

    auto& graphics = Graphics::GraphicsEngine::GetInstance();
    graphics.Initialize(window_.GetHWND(), Math::Vector2Int::HD);

    Graphics::ShaderManager::GetInstance().Initialize(graphics.GetRenderDevice());
    Asset::TextureManager::GetInstance().Initialize(graphics.GetRenderDevice());
    Asset::FontManager::GetInstance().Initialize(graphics.GetRenderDevice());
    Asset::MaterialManager::GetInstance().Initialize(graphics.GetRenderDevice());
    Asset::AssetManager::GetInstance().Initialize(graphics.GetRenderDevice());
    Graphics::GeometryPool::GetInstance().Initialize(graphics.GetRenderDevice());
    Graphics::DebugRenderer::GetInstance().Initialize(graphics.GetRenderDevice());

    // 2. スクリプトの初期化
    auto& scriptHost = Script::ScriptHost::GetInstance();
    if (scriptHost.Initialize()) {
        auto initDelegate = (void(*)(void*, void*))scriptHost.GetMethodDelegate(
            L"ONEngine.Scripting.EngineHost, ONEngine.Scripting", L"Initialize", L"");
        if (initDelegate) initDelegate((void*)LogFromRuntime, &registry_);

        updateDelegate_ = (void(*)(float))scriptHost.GetMethodDelegate(
            L"ONEngine.Scripting.EngineHost, ONEngine.Scripting", L"Update", L"");
        shutdownDelegate_ = (void(*)())scriptHost.GetMethodDelegate(
            L"ONEngine.Scripting.EngineHost, ONEngine.Scripting", L"Shutdown", L"");
    }

    // 3. レンダラーとパイプラインの初期化
    auto& renderer = Graphics::Renderer::GetInstance();
    renderer.Initialize(graphics.GetRenderDevice());

    auto& sm = Graphics::ShaderManager::GetInstance();
    sm.LoadPipelineAsset("Assets/Pipelines/BindlessTest.json");
    sm.LoadPipelineAsset("Assets/Pipelines/Blit.json");
    sm.LoadPipelineAsset("Assets/Pipelines/PostProcess.json");
    sm.LoadPipelineAsset("Assets/Pipelines/BloomThreshold.json");
    sm.LoadPipelineAsset("Assets/Pipelines/Blur.json");
    sm.LoadPipelineAsset("Assets/Pipelines/DebugLine.json");
    sm.LoadPipelineAsset("Assets/Pipelines/Sprite.json");
    sm.LoadPipelineAsset("Assets/Pipelines/Skybox.json");
    sm.LoadPipelineAsset("Assets/Pipelines/Text.json");
    sm.LoadPipelineAsset("Assets/Pipelines/ParticleUpdate.json");
    sm.LoadPipelineAsset("Assets/Pipelines/ParticleRender.json");
    sm.LoadPipelineAsset("Assets/Pipelines/CelShader.json");
    sm.LoadPipelineAsset("Assets/Pipelines/ClusterBuilding.json");
    sm.LoadPipelineAsset("Assets/Pipelines/LightAssignment.json");
    sm.LoadPipelineAsset("Assets/Pipelines/Culling.json");
    sm.LoadPipelineAsset("Assets/Pipelines/CullingReset.json");

    // 4. ECSシステムのインスタンス化
    transformSystem_ = std::make_unique<ECS::TransformSystem>();
    renderSystem_ = std::make_unique<ECS::RenderSystem>();
    cameraSystem_ = std::make_unique<ECS::CameraSystem>();
    lightSystem_ = std::make_unique<ECS::LightSystem>();
    skyboxSystem_ = std::make_unique<ECS::SkyboxSystem>();
    textSystem_ = std::make_unique<ECS::TextSystem>();
    particleSystem_ = std::make_unique<ECS::ParticleSystem>();
    particleSystem_->Initialize(graphics.GetRenderDevice());

    clusteredLightManager_ = std::make_unique<Graphics::ClusteredLightManager>();
    clusteredLightManager_->Initialize(graphics.GetRenderDevice());

    gpuCullingManager_ = std::make_unique<Graphics::GPUCullingManager>();
    gpuCullingManager_->Initialize(graphics.GetRenderDevice());

    // 5. バッファの作成
    pointLightSB_ = std::make_unique<Graphics::StructuredBuffer>();
    pointLightSB_->Create(graphics.GetRenderDevice(), sizeof(GeneratedSchema::PointLightData), 64);
    
    spriteSB_ = std::make_unique<Graphics::StructuredBuffer>();
    spriteSB_->Create(graphics.GetRenderDevice(), sizeof(GeneratedSchema::SpriteData), 1024);

    textSB_ = std::make_unique<Graphics::StructuredBuffer>();
    textSB_->Create(graphics.GetRenderDevice(), sizeof(GeneratedSchema::TextData), 4096);

    // 6. シーンのロード
    Scene::SceneLoader::LoadScene("Assets/Scene/Main.scene", registry_);

    timer_.Reset();
    return true;
}

void Application::Run() {
    while (true) {
        window_.Update();
        if (window_.GetIsProcessEnd()) break;

        timer_.Tick();
        Update(timer_.GetDeltaTime());
        Render();
    }
}

void Application::Update(float dt) {
    if (updateDelegate_) updateDelegate_(dt);

    transformSystem_->Update(registry_);
    cameraSystem_->Reset();
    cameraSystem_->Update(registry_);
    lightSystem_->Reset();
    lightSystem_->Update(registry_);
    skyboxSystem_->Reset();
    skyboxSystem_->Update(registry_);
    textSystem_->Reset();
    textSystem_->Update(registry_);
}

void Application::Render() {
    auto& graphics = Graphics::GraphicsEngine::GetInstance();
    auto& renderer = Graphics::Renderer::GetInstance();
    auto& debug = Graphics::DebugRenderer::GetInstance();
    auto& sm = Graphics::ShaderManager::GetInstance();
    auto& tm = Asset::TextureManager::GetInstance();

    // データ集計
    auto lightRes = lightSystem_->GetResult();
    if (!lightRes.pointLights.empty()) {
        pointLightSB_->Update(lightRes.pointLights.data(), (uint32_t)(lightRes.pointLights.size() * sizeof(GeneratedSchema::PointLightData)));
    }

    ECS::SpriteSystem spriteSystem;
    spriteSystem.Update(registry_);
    auto spriteRes = spriteSystem.GetResult();
    if (!spriteRes.sprites.empty()) {
        spriteSB_->Update(spriteRes.sprites.data(), (uint32_t)(spriteRes.sprites.size() * sizeof(GeneratedSchema::SpriteData)));
    }

    auto textRes = textSystem_->GetResult();
    if (!textRes.charInstances.empty()) {
        textSB_->Update(textRes.charInstances.data(), (uint32_t)(textRes.charInstances.size() * sizeof(GeneratedSchema::TextData)));
    }

    debug.Clear();
    for (int i = -10; i <= 10; ++i) {
        debug.DrawLine({ (float)i * 5, 0.1f, -50 }, { (float)i * 5, 0.1f, 50 }, { 0.5f, 0.5f, 0.5f, 1.0f });
        debug.DrawLine({ -50, 0.1f, (float)i * 5 }, { 50, 0.1f, (float)i * 5 }, { 0.5f, 0.5f, 0.5f, 1.0f });
    }

    renderer.ClearQueue();
    renderSystem_->Update(registry_);

    // 描画実行
    graphics.BeginFrame();
    auto* currentFrameRes = graphics.GetCurrentFrameResource();
    auto* commandList = graphics.GetCommandQueue()->GetCommandList();

    // パーティクルの更新
    particleSystem_->Update(registry_);

    GeneratedSchema::SceneData sceneData;
    float nearZ = 0.1f, farZ = 1000.0f;
    Math::Matrix4x4 proj;

    if (cameraSystem_->HasCamera()) {
        const auto& camRes = cameraSystem_->GetResult();
        sceneData.view = camRes.view;
        sceneData.viewProj = camRes.viewProj;
        sceneData.cameraPos = camRes.position;
        nearZ = camRes.nearZ;
        farZ = camRes.farZ;
        proj = camRes.proj;
    } else {
        auto view = Math::Matrix4x4::MakeLookAtLH({ 0, 20, -50 }, { 0, 0, 0 }, { 0, 1, 0 });
        proj = Math::Matrix4x4::MakePerspectiveFovLH(0.45f, 16.0f/9.0f, 0.1f, 1000.0f);
        sceneData.view = view;
        sceneData.viewProj = view * proj;
        sceneData.cameraPos = { 0, 20, -50 };
    }
    sceneData.dirLightColor = lightRes.dirLightColor;
    sceneData.dirLightIntensity = lightRes.dirLightIntensity;
    sceneData.dirLightDirection = lightRes.dirLightDirection;
    sceneData.numPointLights = (uint32_t)lightRes.pointLights.size();
    sceneData.screenWidth = (float)window_.GetWindowSize().x;
    sceneData.screenHeight = (float)window_.GetWindowSize().y;
    sceneData.nearZ = nearZ;
    sceneData.farZ = farZ;
    currentFrameRes->GetSceneCB()->Update(&sceneData, sizeof(sceneData));

    // クラスタライトカリングの更新
    clusteredLightManager_->BuildClusters(static_cast<ID3D12GraphicsCommandList*>(commandList), proj.Inverse(), window_.GetWindowSize(), nearZ, farZ);
    clusteredLightManager_->AssignLights(commandList, currentFrameRes->GetSceneCB()->GetGPUVirtualAddress(), pointLightSB_->GetResource()->GetGPUVirtualAddress(), sceneData.numPointLights);

    // メッシュ情報の同期 (カリングとExecuteIndirectで使用)
    particleSystem_->UpdateMeshInfoBuffer();

    renderer.Extract();


    graphics.Clear({ 0.7f, 0.7f, 0.7f, 1.0f });
    graphics.ClearDepth();

    Graphics::RenderContext context;
    context.commandList = graphics.GetCommandQueue()->GetCommandList();
    context.sceneCBAddress = currentFrameRes->GetSceneCB()->GetGPUVirtualAddress();
    context.pointLightBufferAddress = pointLightSB_->GetResource()->GetGPUVirtualAddress();
    context.frameIndex = graphics.GetCurrentFrameIndex();
    context.numRenderTargets = 3;
    context.rtvFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    context.rtvFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    context.rtvFormats[2] = DXGI_FORMAT_R32G32_UINT;

    context.lightGridBufferAddress = clusteredLightManager_->GetLightGridBuffer()->GetResource()->GetGPUVirtualAddress();
    context.lightIndexListBufferAddress = clusteredLightManager_->GetLightIndexListBuffer()->GetResource()->GetGPUVirtualAddress();

    context.cullingManager = gpuCullingManager_.get();
    context.meshInfoBufferAddress = particleSystem_->GetMeshInfoBufferAddress();
    if (cameraSystem_->HasCamera()) {
        context.viewProj = cameraSystem_->GetResult().viewProj;
    }

    renderer.RenderZPrepass(context);
    renderer.Render(context);

    Graphics::PipelineStateDesc baseDesc;
    for (uint32_t i = 0; i < context.numRenderTargets; ++i) {
        baseDesc.rtvFormats[i] = context.rtvFormats[i];
    }
    baseDesc.numRenderTargets = context.numRenderTargets;

    // Skybox
    if (skyboxSystem_->HasSkybox()) {
        auto* pso = sm.GetOrCreatePSO("Skybox", baseDesc);
        auto* rootSig = sm.GetRootSignature("Skybox");
        context.commandList->SetGraphicsRootSignature(rootSig->Get());
        context.commandList->SetPipelineState(pso->Get());
        ID3D12DescriptorHeap* heaps[] = { tm.GetSrvHeap()->GetHeap() };
        context.commandList->SetDescriptorHeaps(1, heaps);
        context.commandList->SetGraphicsRootConstantBufferView(rootSig->GetParameterIndex("gSceneData"), context.sceneCBAddress);
        context.commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gSkybox"), tm.GetSrvHeap()->GetGPUHandle(skyboxSystem_->GetTextureIndex()));
        context.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context.commandList->DrawInstanced(36, 1, 0, 0);
    }
    
    // Particle
    particleSystem_->Render(registry_, context);

    // Sprite
    if (!spriteRes.sprites.empty()) {
        auto* pso = sm.GetOrCreatePSO("Sprite", baseDesc);
        auto* rootSig = sm.GetRootSignature("Sprite");
        context.commandList->SetGraphicsRootSignature(rootSig->Get());
        context.commandList->SetPipelineState(pso->Get());
        ID3D12DescriptorHeap* heaps[] = { tm.GetSrvHeap()->GetHeap() };
        context.commandList->SetDescriptorHeaps(1, heaps);
        context.commandList->SetGraphicsRootConstantBufferView(rootSig->GetParameterIndex("gSceneData"), context.sceneCBAddress);
        context.commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gTextures"), tm.GetSrvHeap()->GetGPUHandle(0));
        context.commandList->SetGraphicsRootShaderResourceView(rootSig->GetParameterIndex("gSprites"), spriteSB_->GetResource()->GetGPUVirtualAddress());
        context.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        context.commandList->DrawInstanced(4, (UINT)spriteRes.sprites.size(), 0, 0);
    }

    // Text
    if (!textRes.charInstances.empty()) {
        auto* pso = sm.GetOrCreatePSO("Text", baseDesc);
        auto* rootSig = sm.GetRootSignature("Text");
        context.commandList->SetGraphicsRootSignature(rootSig->Get());
        context.commandList->SetPipelineState(pso->Get());
        ID3D12DescriptorHeap* heaps[] = { tm.GetSrvHeap()->GetHeap() };
        context.commandList->SetDescriptorHeaps(1, heaps);
        context.commandList->SetGraphicsRootConstantBufferView(rootSig->GetParameterIndex("gSceneData"), context.sceneCBAddress);
        context.commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gTextures"), tm.GetSrvHeap()->GetGPUHandle(0));
        context.commandList->SetGraphicsRootShaderResourceView(rootSig->GetParameterIndex("gChars"), textSB_->GetResource()->GetGPUVirtualAddress());
        context.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        context.commandList->DrawInstanced(4, (UINT)textRes.charInstances.size(), 0, 0);
    }

    debug.Render(context);
    graphics.EndFrame();
}

void Application::Shutdown() {
    if (shutdownDelegate_) shutdownDelegate_();

    // 先に GPU の完了を待機しないと、各種リソースの破棄時にクラッシュする可能性がある
    Graphics::GraphicsEngine::GetInstance().Shutdown();

    // 1. 各システムの終了処理
    particleSystem_->Shutdown();
    clusteredLightManager_->Shutdown();
    gpuCullingManager_->Shutdown();
    Graphics::Renderer::GetInstance().Shutdown();
    Graphics::DebugRenderer::GetInstance().Shutdown();
    Asset::FontManager::GetInstance().Shutdown();
    Graphics::GeometryPool::GetInstance().Shutdown();
    Script::ScriptHost::GetInstance().Shutdown();
    Asset::AssetManager::GetInstance().Shutdown();
    Asset::MaterialManager::GetInstance().Shutdown();
    Asset::TextureManager::GetInstance().Shutdown();
    Graphics::ShaderManager::GetInstance().Shutdown();

    // 2. Application が保持する GPU リソースを明示的に破棄
    pointLightSB_.reset();
    spriteSB_.reset();
    textSB_.reset();

    // 3. レジストリをクリア（コンポーネントが保持する GPU リソースを破棄）
    registry_.Clear();

    // 4. 各システムのポインタをリセット
    transformSystem_.reset();
    renderSystem_.reset();
    cameraSystem_.reset();
    lightSystem_.reset();
    skyboxSystem_.reset();
    textSystem_.reset();
    particleSystem_.reset();

    // 5. エンジン内部のバッファも破棄されるように明示的にリセット
    // ※GraphicsEngine 自体はシングルトンだが、保持している unique_ptr をリセット
    // ただし、現在はシングルトン内の unique_ptr を外部からリセットする口がないため、
    // 将来的に必要であれば追加。現状は Shutdown() で GPU 待機のみ。

    window_.Shutdown();
    Console::Shutdown();
}

} // namespace Engine::Core
