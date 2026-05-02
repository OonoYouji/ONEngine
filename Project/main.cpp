#include <Windows.h>
#include <iostream>
#include "Engine/Common/Console.h"
#include "Engine/Core/Window.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Shader/ShaderCompiler.h"
#include "Engine/Graphics/Shader/RootSignature.h"
#include "Engine/Graphics/Shader/PipelineState.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Engine::Core::Window window;
    window.Initialize(L"LoadTest", Engine::Math::Vector2Int::HD);

    Engine::Graphics::GraphicsEngine& graphicsEngine = Engine::Graphics::GraphicsEngine::GetInstance();
    graphicsEngine.Initialize(window.GetHWND(), Engine::Math::Vector2Int::HD);

    // --- ロードテスト開始 ---
    Engine::Graphics::ShaderCompiler compiler;
    compiler.Initialize();

    auto vs = compiler.Compile(L"Assets/Shader/Test/Test.hlsl", L"vs_main", L"vs_6_0");
    auto ps = compiler.Compile(L"Assets/Shader/Test/Test.hlsl", L"ps_main", L"ps_6_0");

    if (vs && ps) {
        Engine::Console::Log("Shader compilation success!");
        
        // Reflection情報の確認
        Engine::Console::Log(std::format("VS CBVs: {}", vs->reflectionData.constantBuffers.size()));
        Engine::Console::Log(std::format("PS SRVs: {}", ps->reflectionData.srvs.size()));

        Engine::Graphics::RootSignature rootSig;
        if (rootSig.Create(graphicsEngine.GetRenderDevice(), { vs->reflectionData, ps->reflectionData })) {
            Engine::Console::Log("RootSignature creation success!");

            Engine::Graphics::PipelineState pso;
            if (pso.Create(graphicsEngine.GetRenderDevice(), &rootSig, vs.get(), ps.get())) {
                Engine::Console::Log("PipelineState creation (Stream) success!");
            }
        }
    }
    // --- ロードテスト終了 ---

    while(true) {
        window.Update();
        if(window.GetIsProcessEnd()) break;

        graphicsEngine.BeginFrame();
        graphicsEngine.Clear({ 0.1f, 0.2f, 0.3f, 1.0f });
        graphicsEngine.EndFrame();
    }

    graphicsEngine.Shutdown();
    window.Shutdown();

    return 0;
}
