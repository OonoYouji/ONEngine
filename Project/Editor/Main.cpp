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
#include "EditorUtils.h"
#include "Engine/Scene/SceneLoader.h"

extern "C" void EcsInterop_LinkForce();

namespace {

void SetupBlenderTheme() {
    auto& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Blender-like Dark Theme (v3.0+)
    colors[ImGuiCol_Text]                   = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled]             = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_Border]                 = ImVec4(0.10f, 0.10f, 0.10f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.14f, 0.44f, 0.70f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.14f, 0.44f, 0.70f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.14f, 0.44f, 0.70f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.14f, 0.44f, 0.70f, 1.00f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.20f, 0.50f, 0.80f, 1.00f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.14f, 0.44f, 0.70f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.14f, 0.44f, 0.70f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.14f, 0.44f, 0.70f, 0.70f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.14f, 0.44f, 0.70f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.14f, 0.44f, 0.70f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    style.WindowRounding    = 4.0f;
    style.ChildRounding     = 4.0f;
    style.FrameRounding     = 4.0f;
    style.PopupRounding     = 4.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding      = 4.0f;
    style.TabRounding       = 4.0f;
    style.WindowTitleAlign  = ImVec2(0.5f, 0.5f);
    style.WindowPadding     = ImVec2(8.0f, 8.0f);
    style.FramePadding      = ImVec2(4.0f, 4.0f);
    style.ItemSpacing       = ImVec2(8.0f, 4.0f);
}

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // リンクを強制して P/Invoke ターゲットをエクスポートテーブルに乗せる
    EcsInterop_LinkForce();

	Engine::Core::Application::CreateInstance();
	auto& app = Engine::Core::Application::GetInstance();

	app.SetEditorMode(true);
	if(!app.Initialize(hInstance, nCmdShow)) {
		return -1;
	}

    // 最後に開いていたシーンを読み込む
    {
        auto& context = Engine::Editor::EditorContext::GetInstance();
        auto& reg = app.GetRegistry();
        std::string scenePath = context.GetCurrentScenePath();
        if (!scenePath.empty() && std::filesystem::exists(scenePath)) {
            Engine::Scene::SceneLoader::LoadScene(scenePath, reg);
        }
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

    // フォント設定 (MPLUSフォント + 日本語対応)
    const char* fontPath = "Assets/Fonts/MPLUSRounded1c-Black.ttf";
    if (io.Fonts->AddFontFromFileTTF(fontPath, 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese())) {
        // フォント読み込み成功
    }

	SetupBlenderTheme();

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
		
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		
		ImGui::Begin("Editor DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(3);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("ファイル (File)")) {
                auto& context = Engine::Editor::EditorContext::GetInstance();
                if (ImGui::MenuItem("シーン保存 (Save Scene)")) {
                    auto& reg = Engine::Core::Application::GetInstance().GetRegistry();
                    std::string path = context.GetCurrentScenePath();
                    if (path.empty()) {
                        auto selected = Engine::Editor::EditorUtils::SaveFileDialog("Scene Files (*.scene)\0*.scene\0");
                        if (selected) {
                            path = *selected;
                            context.SetCurrentScenePath(path);
                        }
                    }
                    if (!path.empty()) {
                        Engine::Scene::SceneLoader::SaveScene(path, reg);
                    }
                }
                if (ImGui::MenuItem("名前を付けて保存 (Save Scene As)")) {
                    auto& reg = Engine::Core::Application::GetInstance().GetRegistry();
                    auto selected = Engine::Editor::EditorUtils::SaveFileDialog("Scene Files (*.scene)\0*.scene\0");
                    if (selected) {
                        context.SetCurrentScenePath(*selected);
                        Engine::Scene::SceneLoader::SaveScene(*selected, reg);
                    }
                }
                if (ImGui::MenuItem("シーン読込 (Load Scene)")) {
                    auto selected = Engine::Editor::EditorUtils::OpenFileDialog("Scene Files (*.scene)\0*.scene\0");
                    if (selected) {
                        auto& reg = Engine::Core::Application::GetInstance().GetRegistry();
                        reg.Clear();
                        context.SetCurrentScenePath(*selected);
                        Engine::Scene::SceneLoader::LoadScene(*selected, reg);
                    }
                }
                ImGui::Separator();
				if (ImGui::MenuItem("終了 (Exit)")) { PostQuitMessage(0); }
				ImGui::EndMenu();
			}

			auto& context = Engine::Editor::EditorContext::GetInstance();
			if (ImGui::BeginMenu("編集 (Edit)")) {
				if (ImGui::Checkbox("ギズモ スナップ (Gizmo Snap)", &context.GetSnapEnabled())) { context.SaveSettings(); }
				ImGui::Separator();
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::InputFloat("移動スナップ (Snap Move)", &context.GetSnapTranslation())) { context.SaveSettings(); }
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::InputFloat("回転スナップ (Snap Rotate)", &context.GetSnapRotation())) { context.SaveSettings(); }
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::InputFloat("スケールスナップ (Snap Scale)", &context.GetSnapScale())) { context.SaveSettings(); }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("ツール (Tools)")) {
				if (ImGui::MenuItem("全てのEntityにTagを付与 (Add Tags to All)")) {
					auto& reg = Engine::Core::Application::GetInstance().GetRegistry();
					reg.GetView<Engine::ECS::Transform>().Each([&](Engine::ECS::Entity e, auto& t) {
						if (!reg.HasComponent<Engine::ECS::Tag>(e)) {
							auto& tag = reg.AddComponent<Engine::ECS::Tag>(e);
							sprintf_s(tag.name, "Entity %u", e);
						}
					});
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		// 各 View のレンダリング
		auto& registry = Engine::Core::Application::GetInstance().GetRegistry();
		
		// エディターカメラの更新
		auto& context = Engine::Editor::EditorContext::GetInstance();
		auto& rawInput = app.GetRawInputService();
		// エディタ操作は ImGui キャプチャを無視して入力を取る (ignoreCapture = true)
		context.GetInputMapper().Update(rawInput, context.GetBindingTable(), context.GetActionMap(), true);
		
		auto& editorCam = context.GetCamera();
		editorCam.Update(ImGui::GetIO().DeltaTime);

		// Engine 側にエディタの状態を通知
		app.SetEditorCameraData(
			editorCam.GetViewMatrix(),
			editorCam.GetViewProjMatrix(),
			editorCam.GetPosition(),
			editorCam.GetNearZ(),
			editorCam.GetFarZ()
		);
		app.SetEditorSelectedEntity(context.GetSelectedEntity());

		hierarchyView.Render(registry);
		inspectorView.Render(registry);
		sceneView.Render();
		projectView.Render();

		ImGui::End();
	});

	app.Run();

	app.WaitForGPU();

    Engine::Editor::EditorContext::GetInstance().SaveSettings();

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	app.Shutdown();
	Engine::Core::Application::DestroyInstance();

	return 0;
}
