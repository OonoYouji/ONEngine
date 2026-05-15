#include "Application.h"
#include "Engine/Common/Console.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Core/Renderer.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Asset/MaterialManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/FontManager.h"
#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Graphics/Resource/GeometryPool.h"
#include "Engine/Graphics/PostProcess/DebugRenderer.h"
#include "Engine/Graphics/PostProcess/PostProcessSystem.h"
#include "Engine/Scene/SceneLoader.h"
#include "Engine/Script/ScriptHost.h"
#include "Engine/ECS/ComponentRegistry.h"
#include "Engine/ECS/Systems/SpriteSystem.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

extern "C" void LogFromRuntime(const char*);

namespace Engine::Core {

Application* Application::instance_ = nullptr;

Application::Application() = default;
Application::~Application() = default;

bool Application::Initialize(HINSTANCE hInstance, int nCmdShow) {
    Console::Initialize();

    Asset::AssetDatabase::CreateInstance();
    Asset::AssetRegistry::CreateInstance();
    ECS::ComponentRegistry::CreateInstance();
    Graphics::GraphicsEngine::CreateInstance();
    Graphics::ShaderManager::CreateInstance();
    Asset::TextureManager::CreateInstance();
    Asset::FontManager::CreateInstance();
    Asset::MaterialManager::CreateInstance();
    Asset::AssetManager::CreateInstance();
    Graphics::GeometryPool::CreateInstance();
    Graphics::DebugRenderer::CreateInstance();
    Script::ScriptHost::CreateInstance();
    Graphics::PostProcessSystem::CreateInstance();
    Graphics::Renderer::CreateInstance();

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

    // --- システムを先に生成 (安全のため) ---
    transformSystem_ = std::make_unique<ECS::TransformSystem>();
    renderSystem_ = std::make_unique<ECS::RenderSystem>();
    cameraSystem_ = std::make_unique<ECS::CameraSystem>();
    lightSystem_ = std::make_unique<ECS::LightSystem>();
    skyboxSystem_ = std::make_unique<ECS::SkyboxSystem>();
    textSystem_ = std::make_unique<ECS::TextSystem>();
    particleSystem_ = std::make_unique<ECS::ParticleSystem>();
    animationSystem_ = std::make_unique<ECS::AnimationSystem>();
    animationSystem_->Initialize(graphics.GetRenderDevice());

    // 2. スクリプトの初期化 (システムの後に実行)
    auto& scriptHost = Script::ScriptHost::GetInstance();
    if (scriptHost.Initialize()) {
        auto initDelegate = (void(*)(void*, void*))scriptHost.GetMethodDelegate(L"ONEngine.Scripting.EngineHost, ONEngine.Scripting", L"Initialize", L"");
        if (initDelegate) initDelegate((void*)LogFromRuntime, &registry_);
        updateDelegate_ = (void(*)())scriptHost.GetMethodDelegate(L"ONEngine.Scripting.EngineHost, ONEngine.Scripting", L"Update", L"");
        shutdownDelegate_ = (void(*)())scriptHost.GetMethodDelegate(L"ONEngine.Scripting.EngineHost, ONEngine.Scripting", L"Shutdown", L"");
    }

    auto& renderer = Graphics::Renderer::GetInstance();
    renderer.Initialize(graphics.GetRenderDevice());

    gpuCullingManager_ = std::make_unique<Graphics::GPUCullingManager>();
    gpuCullingManager_->Initialize(graphics.GetRenderDevice());

    clusteredLightManager_ = std::make_unique<Graphics::ClusteredLightManager>();
    clusteredLightManager_->Initialize(graphics.GetRenderDevice());

    auto& sm = Graphics::ShaderManager::GetInstance();
    sm.LoadPipelineAsset("Assets/Pipelines/DebugLine.json");
    sm.LoadPipelineAsset("Assets/Pipelines/Skybox.json");
    sm.LoadPipelineAsset("Assets/Pipelines/CelShader.json");
    sm.LoadPipelineAsset("Assets/Pipelines/PostProcess.json");
    sm.LoadPipelineAsset("Assets/Pipelines/BindlessTest.json");
    sm.LoadPipelineAsset("Assets/Pipelines/Skinning.json");

    Graphics::PostProcessSystem::GetInstance().Initialize(graphics.GetRenderDevice(), graphics.GetRTVHeap(), graphics.GetSRVHeap(), Math::Vector2Int::HD);

    pointLightSB_ = std::make_unique<Graphics::StructuredBuffer>();
    pointLightSB_->Create(graphics.GetRenderDevice(), sizeof(GeneratedSchema::PointLightData), 64);
    
    spriteSB_ = std::make_unique<Graphics::StructuredBuffer>();
    spriteSB_->Create(graphics.GetRenderDevice(), sizeof(GeneratedSchema::SpriteData), 1024);

    textSB_ = std::make_unique<Graphics::StructuredBuffer>();
    textSB_->Create(graphics.GetRenderDevice(), sizeof(GeneratedSchema::TextData), 4096);

    // 実行時のみデフォルトシーンをロード (エディタの場合は後で個別にロードする)
    if (!isEditorMode_) {
        Scene::SceneLoader::LoadScene("Assets/Scene/Main.scene", registry_);
    }

    timer_.Reset();
    return true;
}

void Application::Run() {
    while (true) {
        window_.Update();
        if (window_.GetIsProcessEnd()) break;
        timer_.Tick();

        rawInputService_.Update(window_.GetHWND());
        inputMapper_.Update(rawInputService_, gameBindingTable_, gameActionMap_);

        if (isEditorMode_) {
#ifndef NDEBUG
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
            for (auto& callback : uiCallbacks_) callback();
#endif
        }

        Update(timer_.GetDeltaTime());
        Render();

        if (isEditorMode_) {
#ifndef NDEBUG
            Graphics::GraphicsEngine::GetInstance().EndFrame();
            
            // SwapChain に対して ImGui を描画するために RTV をセットしクリアする
            auto& graphics = Graphics::GraphicsEngine::GetInstance();
            auto* commandList = graphics.GetCommandQueue()->GetCommandList();
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = graphics.GetSwapChain()->GetRTVHandle();
            commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
            
            // 背景色でクリア (ImGui の外側用)
            float clearColor[] = { 0.1f, 0.15f, 0.2f, 1.0f };
            commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

            ImGui::Render();
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), static_cast<ID3D12GraphicsCommandList*>(commandList));
#endif
        } else {
            Graphics::GraphicsEngine::GetInstance().EndFrame();
        }

        Graphics::GraphicsEngine::GetInstance().Present();
    }
}

void Application::Update(float dt) {
    // スクリプトの実行順序を Transform の sortOrder に合わせる
    if (registry_.HasStorage<ECS::ScriptComponent>()) {
        registry_.GetStorage<ECS::ScriptComponent>().Sort([&](ECS::Entity a, ECS::Entity b) {
            float orderA = registry_.HasComponent<ECS::Transform>(a) ? registry_.GetComponent<ECS::Transform>(a).sortOrder : 0.0f;
            float orderB = registry_.HasComponent<ECS::Transform>(b) ? registry_.GetComponent<ECS::Transform>(b).sortOrder : 0.0f;
            return orderA < orderB;
        });
    }

    if (updateDelegate_) updateDelegate_();
    
    // 全システムにヌルチェックを適用
    if (transformSystem_) transformSystem_->Update(registry_);
    if (cameraSystem_) {
        cameraSystem_->Reset();
        cameraSystem_->Update(registry_);
    }
    if (lightSystem_) {
        lightSystem_->Reset();
        lightSystem_->Update(registry_);
    }
    if (skyboxSystem_) {
        skyboxSystem_->Reset();
        skyboxSystem_->Update(registry_);
    }
    if (textSystem_) {
        textSystem_->Reset();
        textSystem_->Update(registry_);
    }
}

void Application::Render() {
    static uint32_t renderFrameCount = 0;
    renderFrameCount++;

    if (renderFrameCount % 100 == 0) {
        Engine::Console::Log(std::format("Application: Registry Max Entity ID is {}.", registry_.GetMaxEntityId()));
    }

    auto& graphics = Graphics::GraphicsEngine::GetInstance();
    auto* currentFrameRes = graphics.GetCurrentFrameResource();
    auto& renderer = Graphics::Renderer::GetInstance();
    auto& debug = Graphics::DebugRenderer::GetInstance();
    auto& sm = Graphics::ShaderManager::GetInstance();
    auto& am = Asset::AssetManager::GetInstance();

    // 1. フレーム開始
    graphics.BeginFrame();
    auto* commandList = graphics.GetCommandQueue()->GetCommandList();
    uint32_t frameIndex = graphics.GetCurrentFrameIndex();
    
    // カリングカウンタのリセット
    if (gpuCullingManager_) {
        gpuCullingManager_->ResetCounters(static_cast<ID3D12GraphicsCommandList*>(commandList));
    }

    if (animationSystem_) {
        animationSystem_->Update(registry_, static_cast<ID3D12GraphicsCommandList*>(commandList), timer_.GetDeltaTime(), frameIndex);
    }

    graphics.Clear({ 0.5f, 0.7f, 0.9f, 1.0f }); 
    graphics.ClearDepth();

    // 2. 描画リクエストの収集
    renderer.ClearQueue();
    if (renderSystem_) renderSystem_->Update(registry_);

    GeneratedSchema::SceneData sceneData{};
    float aspect = (float)graphics.GetWindowSize().x / (float)graphics.GetWindowSize().y;
    
    if (isEditorMode_) {
        sceneData.view = editorView_;
        sceneData.viewProj = editorViewProj_;
        sceneData.cameraPos = editorCameraPos_;
        sceneData.nearZ = editorNearZ_;
        sceneData.farZ = editorFarZ_;
    }
    else if (cameraSystem_ && cameraSystem_->HasCamera()) {
        const auto& cam = cameraSystem_->GetResult();
        sceneData.view = cam.view;
        sceneData.viewProj = cam.viewProj;
        sceneData.cameraPos = cam.position;
        sceneData.nearZ = cam.nearZ;
        sceneData.farZ = cam.farZ;

        if (renderFrameCount % 100 == 0) {
            Engine::Console::Log(std::format("Application: Camera Pos=({:.2f}, {:.2f}, {:.2f})", cam.position.x, cam.position.y, cam.position.z));
            // 行列の一部を出力して正規性を確認
            Engine::Console::Log(std::format("Application: Camera ViewProj[0][0]={:.2f}, [3][3]={:.2f}", cam.viewProj.m[0][0], cam.viewProj.m[3][3]));
        }
    } else {
        float fov = 0.45f;
        float nearZ = 0.1f;
        float farZ = 1000.0f;
        sceneData.view = Math::Matrix4x4::MakeLookAtLH({ 0, 50, -150 }, { 0, 0, 0 }, { 0, 1, 0 });
        sceneData.viewProj = sceneData.view * Math::Matrix4x4::MakePerspectiveFovLH(fov, aspect, nearZ, farZ);
        sceneData.cameraPos = { 0, 50, -150 };
        sceneData.nearZ = nearZ;
        sceneData.farZ = farZ;
    }

    sceneData.screenWidth = (float)graphics.GetWindowSize().x;
    sceneData.screenHeight = (float)graphics.GetWindowSize().y;
    sceneData.selectedEntityID = editorSelectedEntityID_;

    // ライトの反映 (LightSystemがあれば)
    if (lightSystem_) {
        const auto& light = lightSystem_->GetResult();
        sceneData.dirLightColor = light.dirLightColor;
        sceneData.dirLightIntensity = light.dirLightIntensity;
        sceneData.dirLightDirection = light.dirLightDirection;
        
        // 点光源を StructuredBuffer に転送
        if (!light.pointLights.empty()) {
            pointLightSB_->Update(light.pointLights.data(), static_cast<uint32_t>(light.pointLights.size() * sizeof(GeneratedSchema::PointLightData)));
        }
    } else {
        sceneData.dirLightColor = { 1, 1, 1 };
        sceneData.dirLightIntensity = 1.0f;
        sceneData.dirLightDirection = { 0.5f, -1.0f, 0.5f };
    }

    currentFrameRes->GetSceneCB()->Update(&sceneData, sizeof(sceneData));

    am.UpdateMeshInfoBuffer();
    renderer.Extract();

    // クラスタライトカリングの更新
    if (clusteredLightManager_) {
        // 初回のみクラスタ構築
        static bool clustersBuilt = false;
        if (!clustersBuilt) {
            float fov = (cameraSystem_ && cameraSystem_->HasCamera()) ? cameraSystem_->GetResult().proj.m[1][1] : 0.45f; // もしくは適切な FOV 取得
            if (fov > 10.0f) fov = 2.0f * atanf(1.0f / fov); // Cotangent から Radian へ

            clusteredLightManager_->BuildClusters(
                static_cast<ID3D12GraphicsCommandList*>(commandList),
                Math::Matrix4x4::MakePerspectiveFovLH(fov, aspect, sceneData.nearZ, sceneData.farZ).Inverse(),
                graphics.GetWindowSize(),
                sceneData.nearZ,
                sceneData.farZ
            );
            clustersBuilt = true;
        }

        clusteredLightManager_->AssignLights(
            static_cast<ID3D12GraphicsCommandList*>(commandList),
            currentFrameRes->GetSceneCB()->GetGPUVirtualAddress(),
            pointLightSB_->GetResource()->GetGPUVirtualAddress(),
            lightSystem_ ? static_cast<uint32_t>(lightSystem_->GetResult().pointLights.size()) : 0
        );
    }

    // 3. 描画コンテキスト
    Graphics::RenderContext context;
    context.commandList = static_cast<ID3D12GraphicsCommandList*>(commandList);
    context.sceneCBAddress = currentFrameRes->GetSceneCB()->GetGPUVirtualAddress();
    context.pointLightBufferAddress = pointLightSB_->GetResource()->GetGPUVirtualAddress();
    if (clusteredLightManager_) {
        context.lightGridBufferAddress = clusteredLightManager_->GetLightGridBuffer()->GetResource()->GetGPUVirtualAddress();
        context.lightIndexListBufferAddress = clusteredLightManager_->GetLightIndexListBuffer()->GetResource()->GetGPUVirtualAddress();
    }
    context.frameIndex = graphics.GetCurrentFrameIndex();
    context.numRenderTargets = 3;
    context.rtvFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    context.rtvFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    context.rtvFormats[2] = DXGI_FORMAT_R32G32_UINT;
    context.viewProj = sceneData.viewProj;
    context.animationSystem = animationSystem_.get();
    context.cullingManager = nullptr; // 一時的にカリングを完全に無効化
    context.meshInfoBufferAddress = am.GetMeshInfoBufferAddress();

    // 4. シーン描画
    renderer.Render(context);

    // 5. ポストプロセスへの遷移 (RenderTarget -> ShaderResource)
    graphics.GetMainColorBuffer()->Transition(static_cast<ID3D12GraphicsCommandList*>(commandList), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    graphics.GetNormalBuffer()->Transition(static_cast<ID3D12GraphicsCommandList*>(commandList), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    graphics.GetIDBuffer()->Transition(static_cast<ID3D12GraphicsCommandList*>(commandList), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    graphics.GetFinalColorBuffer()->Transition(static_cast<ID3D12GraphicsCommandList*>(commandList), D3D12_RESOURCE_STATE_RENDER_TARGET);

    // ポストプロセス実行
    Graphics::PostProcessSystem::GetInstance().Render(
        static_cast<ID3D12GraphicsCommandList*>(commandList),
        graphics.GetMainColorBuffer(),
        nullptr, // Bloom (実装省略)
        graphics.GetNormalBuffer(),
        graphics.GetIDBuffer(),
        graphics.GetFinalColorBuffer()->GetRTVHandle()
    );

    // 6. デバッグ描画
    // デバッグ描画を最終バッファに行うため、コンテキストのフォーマットを更新
    context.numRenderTargets = 1;
    context.rtvFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    // 最終バッファと深度バッファをセットしてデバッグ描画
    {
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = graphics.GetFinalColorBuffer()->GetRTVHandle();
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = graphics.GetDSVHeap()->GetCPUHandle(0); // depthBuffer_ の DSV
        static_cast<ID3D12GraphicsCommandList*>(commandList)->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    }

    debug.Clear();
    for (int i = -10; i <= 10; ++i) {
        debug.DrawLine({ (float)i * 5, 0.0f, -50 }, { (float)i * 5, 0.0f, 50 }, { 0.5f, 0.5f, 0.5f, 1.0f });
        debug.DrawLine({ -50, 0.0f, (float)i * 5 }, { 50, 0.0f, (float)i * 5 }, { 0.5f, 0.5f, 0.5f, 1.0f });
    }
    debug.Render(context);

    // 表示用に遷移 (RenderTarget -> ShaderResource)
    graphics.GetFinalColorBuffer()->Transition(static_cast<ID3D12GraphicsCommandList*>(commandList), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

void Application::WaitForGPU() {
    Graphics::GraphicsEngine::GetInstance().Shutdown();
}

void Application::Shutdown() {
    if (shutdownDelegate_) shutdownDelegate_();
    Graphics::GraphicsEngine::GetInstance().Shutdown();
    transformSystem_.reset();
    renderSystem_.reset();
    cameraSystem_.reset();
    lightSystem_.reset();
    skyboxSystem_.reset();
    textSystem_.reset();
    particleSystem_.reset();
    animationSystem_.reset();
    if (gpuCullingManager_) gpuCullingManager_->Shutdown();
    gpuCullingManager_.reset();
    if (clusteredLightManager_) clusteredLightManager_->Shutdown();
    clusteredLightManager_.reset();
    pointLightSB_.reset();
    spriteSB_.reset();
    textSB_.reset();
    registry_.Clear();
    Asset::AssetManager::DestroyInstance();
    Asset::MaterialManager::DestroyInstance();
    Asset::TextureManager::DestroyInstance();
    Asset::FontManager::DestroyInstance();
    Graphics::Renderer::DestroyInstance();
    Graphics::PostProcessSystem::DestroyInstance();
    Graphics::DebugRenderer::DestroyInstance();
    Graphics::GeometryPool::DestroyInstance();
    Script::ScriptHost::DestroyInstance();
    Graphics::ShaderManager::DestroyInstance();
    Graphics::GraphicsEngine::DestroyInstance();
    ECS::ComponentRegistry::DestroyInstance();
    Asset::AssetRegistry::DestroyInstance();
    Asset::AssetDatabase::DestroyInstance();
    window_.Shutdown();
    Application::DestroyInstance();
    Console::Shutdown();
}

} // namespace Engine::Core
