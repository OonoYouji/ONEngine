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
	if(!app.Initialize(hInstance, nCmdShow)) {
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
	// Viewports は PIX キャプチャとヒープ管理の障害になるため、開発中は無効化
	io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;

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

		// Scene View の実装
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Scene View");

		auto& graphics = Engine::Graphics::GraphicsEngine::GetInstance();
		auto* colorBuffer = graphics.GetMainColorBuffer();
		if(colorBuffer) {
			// エンジンのメイン SRV ヒープに存在するテクスチャの GPU ハンドルを渡す
			// ImGui_ImplDX12 は、バックエンドに渡したヒープ内のハンドルであれば Image として表示可能
			D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = colorBuffer->GetSRVHandle();
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

			if(viewportPanelSize.x > 0 && viewportPanelSize.y > 0) {
				ImGui::Image((ImTextureID)srvHandle.ptr, viewportPanelSize);
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	});

	app.Run();

	// 1. GPU の完了を待機
	app.WaitForGPU();

	// 2. 安全になった状態で ImGui の終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// 3. エンジンリソースの破棄
	app.Shutdown();
	Engine::Core::Application::DestroyInstance();

	return 0;
}
//}
