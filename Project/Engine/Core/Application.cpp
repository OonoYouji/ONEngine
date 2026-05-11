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

    // シングルトンの生成
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
            L"ONEngine.Scripting.EngineHost", L"Initialize", L"");
        if (initDelegate) initDelegate((void*)LogFromRuntime, &registry_);

        updateDelegate_ = (void(*)())scriptHost.GetMethodDelegate(
            L"ONEngine.Scripting.EngineHost", L"Update", L"");
        shutdownDelegate_ = (void(*)())scriptHost.GetMethodDelegate(
            L"ONEngine.Scripting.EngineHost", L"Shutdown", L"");
    }

    // 3. レンダラーとパイプラインの初期化
    auto& renderer = Graphics::Renderer::GetInstance();
    renderer.Initialize(graphics.GetRenderDevice());

    auto& sm = Graphics::ShaderManager::GetInstance();
    sm.LoadPipelineAsset("Assets/Pipelines/DebugLine.json");

    // 4. ECSシステムのインスタンス化
    transformSystem_ = std::make_unique<ECS::TransformSystem>();
    renderSystem_ = std::make_unique<ECS::RenderSystem>();
    cameraSystem_ = std::make_unique<ECS::CameraSystem>();
    lightSystem_ = std::make_unique<ECS::LightSystem>();
    skyboxSystem_ = std::make_unique<ECS::SkyboxSystem>();
    textSystem_ = std::make_unique<ECS::TextSystem>();
    particleSystem_ = std::make_unique<ECS::ParticleSystem>();
    animationSystem_ = std::make_unique<ECS::AnimationSystem>();

    Graphics::PostProcessSystem::GetInstance().Initialize(
        graphics.GetRenderDevice(), 
        graphics.GetRTVHeap(), 
        graphics.GetSRVHeap(), 
        Math::Vector2Int::HD);

    // 5. バッファの作成
    pointLightSB_ = std::make_unique<Graphics::StructuredBuffer>();
    pointLightSB_->Create(graphics.GetRenderDevice(), sizeof(GeneratedSchema::PointLightData), 64);
    
    spriteSB_ = std::make_unique<Graphics::StructuredBuffer>();
    spriteSB_->Create(graphics.GetRenderDevice(), sizeof(GeneratedSchema::SpriteData), 1024);

    textSB_ = std::make_unique<Graphics::StructuredBuffer>();
    textSB_->Create(graphics.GetRenderDevice(), sizeof(GeneratedSchema::TextData), 4096);

    timer_.Reset();
    return true;
}

void Application::Run() {
    while (true) {
        window_.Update();
        if (window_.GetIsProcessEnd()) break;

        timer_.Tick();

        if (isEditorMode_) {
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
            for (auto& callback : uiCallbacks_) callback();
        }

        Update(timer_.GetDeltaTime());
        Render();

        if (isEditorMode_) {
            Graphics::GraphicsEngine::GetInstance().EndFrame(); // MRT -> SRV
            ImGui::Render();
            auto* commandList = Graphics::GraphicsEngine::GetInstance().GetCommandQueue()->GetCommandList();
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), static_cast<ID3D12GraphicsCommandList*>(commandList));
        }

        Graphics::GraphicsEngine::GetInstance().Present();
    }
}

void Application::Update(float dt) {
    if (updateDelegate_) updateDelegate_();
    transformSystem_->Update(registry_);
    cameraSystem_->Reset();
    cameraSystem_->Update(registry_);
    lightSystem_->Reset();
    lightSystem_->Update(registry_);
}

void Application::Render() {
    auto& graphics = Graphics::GraphicsEngine::GetInstance();
    auto* currentFrameRes = graphics.GetCurrentFrameResource();
    auto& debug = Graphics::DebugRenderer::GetInstance();

    debug.Clear();

    // --- 描画前準備フェーズ ---
    graphics.BeginFrame();
    auto* commandList = graphics.GetCommandQueue()->GetCommandList();
    
    graphics.Clear({ 0.1f, 0.15f, 0.3f, 1.0f }); 
    graphics.ClearDepth();

    // 診断用コンテキスト
    Graphics::RenderContext context;
    context.commandList = static_cast<ID3D12GraphicsCommandList*>(commandList);
    context.sceneCBAddress = currentFrameRes->GetSceneCB()->GetGPUVirtualAddress();
    context.frameIndex = graphics.GetCurrentFrameIndex();
    
    debug.Render(context);
}

void Application::Shutdown() {
    if (shutdownDelegate_) shutdownDelegate_();

    // 1. GPUの完了を待機
    auto& graphics = Graphics::GraphicsEngine::GetInstance();
    graphics.Shutdown();

    // 2. 全ての「リソースを保持する可能性があるもの」を先に破棄する
    transformSystem_.reset();
    renderSystem_.reset();
    cameraSystem_.reset();
    lightSystem_.reset();
    skyboxSystem_.reset();
    textSystem_.reset();
    particleSystem_.reset();
    animationSystem_.reset();

    pointLightSB_.reset();
    spriteSB_.reset();
    textSB_.reset();

    registry_.Clear();

    // 3. アセットマネージャの破棄 (モデルやテクスチャの所有権を解放)
    Asset::AssetManager::DestroyInstance();
    Asset::MaterialManager::DestroyInstance();
    Asset::TextureManager::DestroyInstance();
    Asset::FontManager::DestroyInstance();

    // 4. その他のシングルトンの破棄
    Graphics::Renderer::DestroyInstance();
    Graphics::PostProcessSystem::DestroyInstance();
    Graphics::DebugRenderer::DestroyInstance();
    Graphics::GeometryPool::DestroyInstance();
    Script::ScriptHost::DestroyInstance();
    Graphics::ShaderManager::DestroyInstance();

    // 5. 最後にメモリ管理を行っている GraphicsEngine を破棄
    Graphics::GraphicsEngine::DestroyInstance();

    // 6. システム基盤の破棄
    ECS::ComponentRegistry::DestroyInstance();
    Asset::AssetRegistry::DestroyInstance();
    Asset::AssetDatabase::DestroyInstance();

    window_.Shutdown();
    Console::Shutdown();
}

} // namespace Engine::Core
