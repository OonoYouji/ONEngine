#include "Engine/Core/Application.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    Engine::Core::Application::CreateInstance();
    auto& app = Engine::Core::Application::GetInstance();

    app.SetEditorMode(true);
    if (!app.Initialize(hInstance, nCmdShow)) {
        return -1;
    }

    // ImGui の初期化
    auto& graphics = Engine::Graphics::GraphicsEngine::GetInstance();
    auto* device = graphics.GetRenderDevice();
    auto* srvHeap = graphics.GetSRVHeap();

    // ImGui 用の Descriptor を 1 つ確保
    UINT handleIndex = 0; // 本来はマネージャー経由で取得すべきだが、初期化時なので 0 を使用 (仮)
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // 診断のため一時無効化

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(app.GetHWND());
    ImGui_ImplDX12_Init(
        device->GetDevice(),
        3, // Triple Buffering
        DXGI_FORMAT_R8G8B8A8_UNORM, // SwapChain format (Standard)
        srvHeap->GetHeap(),
        graphics.GetImGuiCPUHandle(),
        graphics.GetImGuiGPUHandle()
    );

    // テスト用の UI 登録
    app.RegisterUICallback([]() {
        ImGui::Begin("Hierarchy");
        ImGui::Text("Entity List");
        ImGui::End();

        ImGui::Begin("Inspector");
        ImGui::Text("Properties");
        ImGui::End();
    });

    app.Run();

    // ImGui の終了処理
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    app.Shutdown();
    Engine::Core::Application::DestroyInstance();

    return 0;
}
