#include <Windows.h>
#include "Engine/Core/Window.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Common/Console.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    Engine::Console::Initialize();


    Engine::Core::Window window;
    window.Initialize(L"PipelineTemplateTest", Engine::Math::Vector2Int::HD);

    auto& graphicsEngine = Engine::Graphics::GraphicsEngine::GetInstance();
    graphicsEngine.Initialize(window.GetHWND(), Engine::Math::Vector2Int::HD);

    auto& shaderManager = Engine::Graphics::ShaderManager::GetInstance();
    shaderManager.Initialize(graphicsEngine.GetRenderDevice());

    // --- テンプレートロードテスト ---
    if (shaderManager.LoadPipelineAsset("Assets/Pipelines/TestTemplate.json")) {
        // 1. 初回作成
        Engine::Graphics::PipelineStateDesc desc;
        desc.blendEnable = true;
        auto* pso1 = shaderManager.GetOrCreatePSO("TestTemplate", desc);
        if (pso1) Engine::Console::Log("PSO 1 (Transparent) created!");

        // 2. キャッシュからの取得
        auto* pso1_cached = shaderManager.GetOrCreatePSO("TestTemplate", desc);
        if (pso1 == pso1_cached) Engine::Console::Log("PSO 1 cached successfully!");

        // 3. 別の設定（不透明）で作成
        desc.blendEnable = false;
        auto* pso2 = shaderManager.GetOrCreatePSO("TestTemplate", desc);
        if (pso2 && pso1 != pso2) Engine::Console::Log("PSO 2 (Opaque) created independently!");
    }

    while(true) {
        window.Update();
        if(window.GetIsProcessEnd()) break;

        graphicsEngine.BeginFrame();
        graphicsEngine.Clear({ 0.1f, 0.2f, 0.3f, 1.0f });
        graphicsEngine.EndFrame();
    }

    shaderManager.Shutdown();
    graphicsEngine.Shutdown();
    window.Shutdown();

    Engine::Console::Shutdown();

    return 0;
}
