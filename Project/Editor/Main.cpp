#include "Engine/Core/Application.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

// Editor Views
#include "HierarchyView.h"
#include "InspectorView.h"
#include "SceneView.h"
#include "ProjectView.h"
#include "EditorContext.h"

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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(app.GetHWND());
	ImGui_ImplDX12_Init(
		device->GetDevice(),
		3, // Triple Buffering
		DXGI_FORMAT_R8G8B8A8_UNORM,
		srvHeap->GetHeap(),
		graphics.GetImGuiCPUHandle(),
		graphics.GetImGuiGPUHandle()
	);

	// View のインスタンス化
	static Engine::Editor::HierarchyView hierarchyView;
	static Engine::Editor::InspectorView inspectorView;
	static Engine::Editor::SceneView sceneView;
	static Engine::Editor::ProjectView projectView;

	// UI 登録
	app.RegisterUICallback([&]() {
		// DockSpace の設定
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		
		ImGui::Begin("Editor DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(3);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit")) { /* TODO */ }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// 各 View のレンダリング
		auto& registry = Engine::Core::Application::GetInstance().GetRegistry();
		
		// エディターカメラの更新
		auto& context = Engine::Editor::EditorContext::GetInstance();
		auto& rawInput = app.GetRawInputService();
		// エディタ操作は ImGui キャプチャを無視して入力を取る (ignoreCapture = true)
		context.GetInputMapper().Update(rawInput, context.GetBindingTable(), context.GetActionMap(), true);
		
		context.GetCamera().Update(ImGui::GetIO().DeltaTime);

		hierarchyView.Render(registry);
		inspectorView.Render(registry);
		sceneView.Render();
		projectView.Render();

		ImGui::End();
	});

	app.Run();

	app.WaitForGPU();

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	app.Shutdown();
	Engine::Core::Application::DestroyInstance();

	return 0;
}
//}
