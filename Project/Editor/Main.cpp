#include "Engine/Core/Application.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

// Editor Views
#include "HierarchyView.h"
#include "InspectorView.h"
#include "SceneView.h"
#include "ProjectView.h"
#include "ConsoleView.h"
#include "EditorContext.h"
#include "EditorUtils.h"
#include "CommandHistory.h"
#include "Engine/Scene/SceneLoader.h"

extern "C" void EcsInterop_LinkForce();

namespace {

void SetupBlenderTheme() {
    auto& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Blender 3.x/4.x Dark Theme Colors (Refined)
    const ImVec4 b_gray_darkest  = ImVec4(0.12f, 0.12f, 0.12f, 1.00f); // #1E1E1E
    const ImVec4 b_gray_dark     = ImVec4(0.18f, 0.18f, 0.18f, 1.00f); // #2E2E2E
    const ImVec4 b_gray_mid      = ImVec4(0.24f, 0.24f, 0.24f, 1.00f); // #3D3D3D
    const ImVec4 b_gray_light    = ImVec4(0.35f, 0.35f, 0.35f, 1.00f); // #595959
    const ImVec4 b_blue_accent   = ImVec4(0.18f, 0.45f, 0.70f, 1.00f); // Blender Blue
    const ImVec4 b_orange_accent = ImVec4(0.90f, 0.52f, 0.36f, 1.00f); // Blender Orange
    const ImVec4 b_text          = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    const ImVec4 b_text_dim      = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

    colors[ImGuiCol_Text]                   = b_text;
    colors[ImGuiCol_TextDisabled]           = b_text_dim;
    colors[ImGuiCol_WindowBg]               = b_gray_dark;
    colors[ImGuiCol_ChildBg]                = b_gray_dark;
    colors[ImGuiCol_PopupBg]                = b_gray_darkest;
    colors[ImGuiCol_Border]                 = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = b_gray_mid;
    colors[ImGuiCol_FrameBgHovered]         = b_gray_light;
    colors[ImGuiCol_FrameBgActive]          = b_blue_accent;
    colors[ImGuiCol_TitleBg]                = b_gray_darkest;
    colors[ImGuiCol_TitleBgActive]          = b_gray_darkest;
    colors[ImGuiCol_TitleBgCollapsed]       = b_gray_darkest;
    colors[ImGuiCol_MenuBarBg]              = b_gray_darkest;
    colors[ImGuiCol_ScrollbarBg]            = b_gray_dark;
    colors[ImGuiCol_ScrollbarGrab]          = b_gray_light;
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = b_blue_accent;
    colors[ImGuiCol_CheckMark]              = b_blue_accent;
    colors[ImGuiCol_SliderGrab]             = b_gray_light;
    colors[ImGuiCol_SliderGrabActive]       = b_blue_accent;
    colors[ImGuiCol_Button]                 = b_gray_mid;
    colors[ImGuiCol_ButtonHovered]          = b_gray_light;
    colors[ImGuiCol_ButtonActive]           = b_blue_accent;
    colors[ImGuiCol_Header]                 = b_gray_mid;
    colors[ImGuiCol_HeaderHovered]          = b_gray_light;
    colors[ImGuiCol_HeaderActive]           = b_blue_accent;
    colors[ImGuiCol_Separator]              = b_gray_darkest;
    colors[ImGuiCol_SeparatorHovered]       = b_gray_light;
    colors[ImGuiCol_SeparatorActive]        = b_blue_accent;
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered]      = b_gray_light;
    colors[ImGuiCol_ResizeGripActive]       = b_blue_accent;
    colors[ImGuiCol_Tab]                    = b_gray_darkest;
    colors[ImGuiCol_TabHovered]             = b_gray_mid;
    colors[ImGuiCol_TabActive]              = b_gray_mid;
    colors[ImGuiCol_TabUnfocused]           = b_gray_darkest;
    colors[ImGuiCol_TabUnfocusedActive]     = b_gray_darkest;
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.28f, 0.45f, 0.70f, 0.50f);
    colors[ImGuiCol_DockingEmptyBg]         = b_gray_darkest;
    colors[ImGuiCol_PlotLines]              = b_text_dim;
    colors[ImGuiCol_PlotLinesHovered]       = b_orange_accent;
    colors[ImGuiCol_PlotHistogram]          = b_orange_accent;
    colors[ImGuiCol_PlotHistogramHovered]   = b_blue_accent;
    colors[ImGuiCol_TableHeaderBg]          = b_gray_mid;
    colors[ImGuiCol_TableBorderStrong]      = b_gray_darkest;
    colors[ImGuiCol_TableBorderLight]       = b_gray_mid;
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);
    colors[ImGuiCol_TextSelectedBg]         = b_blue_accent;
    colors[ImGuiCol_DragDropTarget]         = b_orange_accent;
    colors[ImGuiCol_NavHighlight]           = b_blue_accent;

    style.WindowRounding    = 0.0f;
    style.ChildRounding     = 0.0f;
    style.FrameRounding     = 3.0f;
    style.PopupRounding     = 0.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding      = 3.0f;
    style.TabRounding       = 4.0f;
    style.WindowTitleAlign  = ImVec2(0.5f, 0.5f);
    style.WindowPadding     = ImVec2(4.0f, 4.0f);
    style.FramePadding      = ImVec2(6.0f, 3.0f);
    style.ItemSpacing       = ImVec2(4.0f, 4.0f);
    style.ScrollbarSize     = 14.0f;
    style.GrabMinSize       = 10.0f;
}

void SetupBlenderLayout(const char* dockspace_name, ImVec2 size) {
    if (size.x <= 0 || size.y <= 0) {
        Engine::Console::LogWarning(std::format("Main: SetupBlenderLayout skipped for {} due to invalid size: {}x{}", dockspace_name, size.x, size.y));
        return;
    }

    ImGuiID dockspace_id = ImGui::GetID(dockspace_name);
    Engine::Console::Log(std::format("Main: Initializing Blender Layout for {} (Size: {}x{})...", dockspace_name, size.x, size.y));
    
    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, size);

    ImGuiID dock_id_main = dockspace_id;
    
    // モード名に応じてデフォルトレイアウトを微調整（将来的にモード別構成にするための準備）
    std::string nameStr = dockspace_name;
    if (nameStr.find("Material") != std::string::npos) {
        // Materialモード用の配置例: 左にプレビュー、下にプロパティなど（現在は共通）
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Right, 0.25f, nullptr, &dock_id_main);
        ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Down, 0.30f, nullptr, &dock_id_main);
        
        ImGui::DockBuilderDockWindow("Properties", dock_id_right);
        ImGui::DockBuilderDockWindow("Asset Browser", dock_id_bottom);
        ImGui::DockBuilderDockWindow("Console", dock_id_bottom);
        ImGui::DockBuilderDockWindow("Viewport (Material)", dock_id_main);
    } else if (nameStr.find("Prefab") != std::string::npos) {
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Right, 0.20f, nullptr, &dock_id_main);
        ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Down, 0.25f, nullptr, &dock_id_main);
        ImGuiID dock_id_right_bottom = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Down, 0.60f, nullptr, &dock_id_right);

        ImGui::DockBuilderDockWindow("Viewport (Prefab)", dock_id_main);
        ImGui::DockBuilderDockWindow("Outliner", dock_id_right);
        ImGui::DockBuilderDockWindow("Properties", dock_id_right_bottom);
        ImGui::DockBuilderDockWindow("Asset Browser", dock_id_bottom);
        ImGui::DockBuilderDockWindow("Console", dock_id_bottom);
    } else {
        // Scene モード用の標準配置
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Right, 0.20f, nullptr, &dock_id_main);
        ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Down, 0.25f, nullptr, &dock_id_main);
        ImGuiID dock_id_right_bottom = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Down, 0.60f, nullptr, &dock_id_right);

        ImGui::DockBuilderDockWindow("Viewport (Scene)", dock_id_main);
        ImGui::DockBuilderDockWindow("Outliner", dock_id_right);
        ImGui::DockBuilderDockWindow("Properties", dock_id_right_bottom);
        ImGui::DockBuilderDockWindow("Asset Browser", dock_id_bottom);
        ImGui::DockBuilderDockWindow("Console", dock_id_bottom);
    }

    ImGui::DockBuilderFinish(dockspace_id);
    Engine::Console::Log(std::format("Main: Layout {} initialized successfully.", dockspace_name));
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
            reg.Clear(); // 起動時のクリーンアップ
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
	static Engine::Editor::ConsoleView consoleView;

	// UI 登録
	app.RegisterUICallback([&]() {
        auto& context = Engine::Editor::EditorContext::GetInstance();
        const auto& tabs = context.GetTabs();

        // 1. Main Menu Bar (Updates Viewport Work Area)
		if (ImGui::BeginMainMenuBar()) {
            // --- Standard Menus ---
			if (ImGui::BeginMenu("File")) {
                std::string sceneInitialDir = (std::filesystem::current_path() / "Assets" / "Scene").string();
                std::replace(sceneInitialDir.begin(), sceneInitialDir.end(), '/', '\\');

                if (ImGui::MenuItem("Save Scene")) {
                    auto& reg = Engine::Core::Application::GetInstance().GetRegistry();
                    std::string path = context.GetCurrentScenePath();
                    if (path.empty()) {
                        auto selected = Engine::Editor::EditorUtils::SaveFileDialog("Scene Files (*.scene)\0*.scene\0", sceneInitialDir.c_str());
                        if (selected) {
                            path = *selected;
                            context.SetCurrentScenePath(path);
                        }
                    }
                    if (!path.empty()) {
                        Engine::Scene::SceneLoader::SaveScene(path, reg);
                    }
                }
                if (ImGui::MenuItem("Save Scene As")) {
                    auto& reg = Engine::Core::Application::GetInstance().GetRegistry();
                    auto selected = Engine::Editor::EditorUtils::SaveFileDialog("Scene Files (*.scene)\0*.scene\0", sceneInitialDir.c_str());
                    if (selected) {
                        context.SetCurrentScenePath(*selected);
                        Engine::Scene::SceneLoader::SaveScene(*selected, reg);
                    }
                }
                if (ImGui::MenuItem("Load Scene")) {
                    auto selected = Engine::Editor::EditorUtils::OpenFileDialog("Scene Files (*.scene)\0*.scene\0", sceneInitialDir.c_str());
                    if (selected) {
                        auto& reg = Engine::Core::Application::GetInstance().GetRegistry();
                        reg.Clear();
                        context.SetCurrentScenePath(*selected);
                        Engine::Scene::SceneLoader::LoadScene(*selected, reg);
                    }
                }
                ImGui::Separator();
				if (ImGui::MenuItem("Exit")) { PostQuitMessage(0); }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Create Empty Entity", "Ctrl+N")) {
                    auto& reg = Engine::Core::Application::GetInstance().GetRegistry();
                    Engine::ECS::Entity entity = reg.CreateEntity();
                    
                    auto& tag = reg.AddComponent<Engine::ECS::Tag>(entity);
                    strcpy_s(tag.name, sizeof(tag.name), "New Entity");
                    tag.isActive = 1;

                    auto& transform = reg.AddComponent<Engine::ECS::Transform>(entity);
                    transform.isEnabled = 1;

                    context.SetSelectedEntity(entity);
                }
                ImGui::Separator();
				if (ImGui::Checkbox("Gizmo Snap", &context.GetSnapEnabled())) { context.SaveSettings(); }
				ImGui::Separator();
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::InputFloat("Snap Move", &context.GetSnapTranslation())) { context.SaveSettings(); }
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::InputFloat("Snap Rotate", &context.GetSnapRotation())) { context.SaveSettings(); }
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::InputFloat("Snap Scale", &context.GetSnapScale())) { context.SaveSettings(); }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window")) {
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                if (ImGui::MenuItem("Reset Layout")) {
                    auto activeTabIndex = context.GetActiveTabIndex();
                    auto activeTabType = tabs[activeTabIndex].type;
                    std::string modeName = "Scene";
                    if (activeTabType == Engine::Editor::EditorContext::TabType::Prefab) modeName = "Prefab";
                    else if (activeTabType == Engine::Editor::EditorContext::TabType::Material) modeName = "Material";
                    std::string dockspaceName = "DockSpace_" + modeName;

                    float menuBarHeight = ImGui::GetFrameHeight();
                    SetupBlenderLayout(dockspaceName.c_str(), ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - menuBarHeight));
                }
                ImGui::Separator();
				if (ImGui::MenuItem("Hierarchy", nullptr, context.GetShowHierarchy())) { context.GetShowHierarchy() = !context.GetShowHierarchy(); context.SaveSettings(); }
				if (ImGui::MenuItem("Inspector", nullptr, context.GetShowInspector())) { context.GetShowInspector() = !context.GetShowInspector(); context.SaveSettings(); }
				if (ImGui::MenuItem("Scene View", nullptr, context.GetShowSceneView())) { context.GetShowSceneView() = !context.GetShowSceneView(); context.SaveSettings(); }
				if (ImGui::MenuItem("Project", nullptr, context.GetShowProject())) { context.GetShowProject() = !context.GetShowProject(); context.SaveSettings(); }
				if (ImGui::MenuItem("Console", nullptr, context.GetShowConsole())) { context.GetShowConsole() = !context.GetShowConsole(); context.SaveSettings(); }
				ImGui::EndMenu();
			}

            ImGui::Separator();

            // --- Contextual Menus (Scene, Prefab, Material) ---
            auto activeTabIndex = context.GetActiveTabIndex();
            auto activeTabType = tabs[activeTabIndex].type;

            if (ImGui::BeginMenu("Scene", true)) {
                Engine::Console::Log("Main: 'Scene' menu opened.");
                // Find or switch to Scene tab
                for (int i = 0; i < (int)tabs.size(); ++i) {
                    if (tabs[i].type == Engine::Editor::EditorContext::TabType::Scene) {
                        bool isActive = (activeTabIndex == i);
                        if (ImGui::MenuItem("Switch to Scene Mode", nullptr, isActive)) {
                            Engine::Console::Log(std::format("Main: MenuItem 'Switch to Scene Mode' clicked. Switching to tab index {}.", i));
                            context.SetActiveTab(i);
                            context.GetShowSceneView() = true; // Viewportを表示
                            Engine::Console::Log("Main: Switched to Scene Tab and set ShowSceneView to true.");
                        }
                        break;
                    }
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Show Scene View", nullptr, context.GetShowSceneView())) {
                    context.GetShowSceneView() = !context.GetShowSceneView();
                    Engine::Console::Log(std::format("Main: MenuItem 'Show Scene View' toggled to {}.", context.GetShowSceneView()));
                }
                ImGui::Separator();
                if (ImGui::MenuItem("New Scene")) { Engine::Console::Log("Main: MenuItem 'New Scene' clicked."); }
                if (ImGui::MenuItem("Open Scene...")) { Engine::Console::Log("Main: MenuItem 'Open Scene' clicked."); }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Prefab", true)) {
                Engine::Console::Log("Main: 'Prefab' menu opened.");
                bool hasPrefabTab = false;
                for (int i = 0; i < (int)tabs.size(); ++i) {
                    if (tabs[i].type == Engine::Editor::EditorContext::TabType::Prefab) {
                        bool isActive = (activeTabIndex == i);
                        if (ImGui::MenuItem(tabs[i].name.c_str(), nullptr, isActive)) {
                            Engine::Console::Log(std::format("Main: MenuItem Prefab '{}' clicked. Switching to tab index {}.", tabs[i].name, i));
                            context.SetActiveTab(i);
                            context.GetShowSceneView() = true; 
                        }
                        hasPrefabTab = true;
                    }
                }
                if (!hasPrefabTab) ImGui::TextDisabled("No Prefab Open");
                ImGui::Separator();
                if (ImGui::MenuItem("Show Prefab View", nullptr, context.GetShowSceneView())) {
                    context.GetShowSceneView() = !context.GetShowSceneView();
                    Engine::Console::Log(std::format("Main: MenuItem 'Show Prefab View' toggled to {}.", context.GetShowSceneView()));
                }
                ImGui::Separator();
                if (ImGui::MenuItem("New Prefab")) { Engine::Console::Log("Main: MenuItem 'New Prefab' clicked."); }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Material", true)) {
                Engine::Console::Log("Main: 'Material' menu opened.");
                bool hasMaterialTab = false;
                for (int i = 0; i < (int)tabs.size(); ++i) {
                    if (tabs[i].type == Engine::Editor::EditorContext::TabType::Material) {
                        bool isActive = (activeTabIndex == i);
                        if (ImGui::MenuItem(tabs[i].name.c_str(), nullptr, isActive)) {
                            Engine::Console::Log(std::format("Main: MenuItem Material '{}' clicked. Switching to tab index {}.", tabs[i].name, i));
                            context.SetActiveTab(i);
                            context.GetShowSceneView() = true;
                        }
                        hasMaterialTab = true;
                    }
                }
                if (!hasMaterialTab) ImGui::TextDisabled("No Material Open");
                ImGui::Separator();
                if (ImGui::MenuItem("Show Material View", nullptr, context.GetShowSceneView())) {
                    context.GetShowSceneView() = !context.GetShowSceneView();
                    Engine::Console::Log(std::format("Main: MenuItem 'Show Material View' toggled to {}.", context.GetShowSceneView()));
                }
                ImGui::Separator();
                if (ImGui::MenuItem("New Material")) { Engine::Console::Log("Main: MenuItem 'New Material' clicked."); }
                ImGui::EndMenu();
            }

			ImGui::EndMainMenuBar();
		}

		ImGuiViewport* viewport = ImGui::GetMainViewport();

        // 2. Root Workspace Window
        auto activeTabIndex = context.GetActiveTabIndex();
        auto activeTabType = tabs[activeTabIndex].type;
        std::string modeName = "Scene";
        std::string viewportTitle = "Viewport (Scene)";
        if (activeTabType == Engine::Editor::EditorContext::TabType::Prefab) {
            modeName = "Prefab";
            viewportTitle = "Viewport (Prefab)";
        }
        else if (activeTabType == Engine::Editor::EditorContext::TabType::Material) {
            modeName = "Material";
            viewportTitle = "Viewport (Material)";
        }

        const char* rootWindowName = "RootWorkspace";
        std::string dockspaceName = "DockSpace_" + modeName;

		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		
		bool rootVisible = ImGui::Begin(rootWindowName, nullptr, window_flags);
		ImGui::PopStyleVar(3);

        if (rootVisible) {
            // --- 1. Blender-style Mode Bar (Fixed height child) ---
            float modeBarHeight = ImGui::GetFrameHeight() + 4.0f;
            ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
            
            if (ImGui::BeginChild("ModeBar", ImVec2(0, modeBarHeight), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar)) {
                if (ImGui::BeginMenuBar()) {
                    // --- Mode Selector ---
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
                    ImGui::SetNextItemWidth(120.0f);
                    if (ImGui::BeginCombo("##Mode", (modeName + " Mode").c_str())) {
                        if (ImGui::Selectable("Scene Mode", activeTabType == Engine::Editor::EditorContext::TabType::Scene)) {
                             for (int i = 0; i < (int)tabs.size(); ++i) { if (tabs[i].type == Engine::Editor::EditorContext::TabType::Scene) { context.SetActiveTab(i); break; } }
                        }
                        if (ImGui::Selectable("Prefab Mode", activeTabType == Engine::Editor::EditorContext::TabType::Prefab)) {
                             /* ... logic to open or switch to prefab ... */
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::PopStyleVar();

                    ImGui::Separator();

                    // --- Blender Menus (View, Select, Add, Object) ---
                    if (activeTabType == Engine::Editor::EditorContext::TabType::Scene) {
                        if (ImGui::BeginMenu("View")) {
                            if (ImGui::MenuItem("Tool Shelf", "T")) {}
                            if (ImGui::MenuItem("Sidebar", "N")) {}
                            ImGui::EndMenu();
                        }
                        if (ImGui::BeginMenu("Select")) {
                            if (ImGui::MenuItem("All", "A")) {}
                            if (ImGui::MenuItem("None", "Alt+A")) { context.ClearSelection(); }
                            if (ImGui::MenuItem("Invert", "Ctrl+I")) {}
                            ImGui::EndMenu();
                        }
                        if (ImGui::BeginMenu("Add")) {
                            if (ImGui::BeginMenu("Mesh")) {
                                if (ImGui::MenuItem("Cube")) {}
                                if (ImGui::MenuItem("Sphere")) {}
                                ImGui::EndMenu();
                            }
                            if (ImGui::MenuItem("Empty")) {
                                auto& reg = Engine::Core::Application::GetInstance().GetRegistry();
                                Engine::ECS::Entity entity = reg.CreateEntity();
                                auto& tag = reg.AddComponent<Engine::ECS::Tag>(entity);
                                strcpy_s(tag.name, sizeof(tag.name), "Empty");
                                context.SetSelectedEntity(entity);
                            }
                            ImGui::EndMenu();
                        }
                        if (ImGui::BeginMenu("Object")) {
                            if (ImGui::MenuItem("Transform")) {}
                            if (ImGui::MenuItem("Set Parent")) {}
                            ImGui::Separator();
                            if (ImGui::MenuItem("Delete", "Del")) { /* ... */ }
                            ImGui::EndMenu();
                        }
                    }

                    // --- Right side: Playback controls ---
                    float playbackWidth = 100.0f;
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - playbackWidth);
                    if (ImGui::Button(context.IsPlaying() ? "Stop" : "Play")) {
                        if (context.IsPlaying()) context.Stop(); else context.Play();
                    }

                    ImGui::EndMenuBar();
                }
                ImGui::EndChild();
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();

            // --- 2. Docking Area (Remaining space) ---
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            if (ImGui::BeginChild("DockingArea", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar)) {
                ImGuiID dockspace_id = ImGui::GetID(dockspaceName.c_str());
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

                // Initialize layout if needed
                static std::set<std::string> initializedLayouts;
                ImVec2 dockSize = ImGui::GetContentRegionAvail();
                if (initializedLayouts.find(dockspaceName) == initializedLayouts.end()) {
                    if (dockSize.x > 100.0f && dockSize.y > 100.0f) {
                        SetupBlenderLayout(dockspaceName.c_str(), dockSize);
                        initializedLayouts.insert(dockspaceName);
                    }
                }
                ImGui::EndChild();
            }
            ImGui::PopStyleVar();
        }
        ImGui::End(); // End Root Workspace Window

		// 各 View のレンダリング
		auto& registry = Engine::Core::Application::GetInstance().GetRegistry();
		
		// エディターカメラの更新
		auto& rawInput = app.GetRawInputService();
        auto& editorCam = context.GetCamera();

		// エディタ操作は ImGui キャプチャを無視して入力を取る (ignoreCapture = true)
		context.GetInputMapper().Update(rawInput, context.GetBindingTable(), context.GetActionMap(), true);
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

        // 各ビューの表示フラグの状態を保持して、変更があったら保存する
        bool prevHierarchy = context.GetShowHierarchy();
        bool prevInspector = context.GetShowInspector();
        bool prevSceneView = context.GetShowSceneView();
        bool prevProject = context.GetShowProject();
        bool prevConsole = context.GetShowConsole();

        if (context.GetShowHierarchy()) hierarchyView.Render(registry, &context.GetShowHierarchy());
		if (context.GetShowInspector()) inspectorView.Render(registry, &context.GetShowInspector());
		if (context.GetShowSceneView()) {
            // Engine::Console::Log(std::format("Main: Rendering SceneView with title: {}", viewportTitle));
            sceneView.Render(viewportTitle.c_str(), &context.GetShowSceneView());
        }
		if (context.GetShowProject()) projectView.Render(&context.GetShowProject());
		if (context.GetShowConsole()) consoleView.Render(&context.GetShowConsole());

        if (prevHierarchy != context.GetShowHierarchy() || prevInspector != context.GetShowInspector() ||
            prevSceneView != context.GetShowSceneView() || prevProject != context.GetShowProject() ||
            prevConsole != context.GetShowConsole()) {
            context.SaveSettings();
        }

        // エンジンの InputMapper を使用したショートカット処理
        if (!ImGui::GetIO().WantCaptureKeyboard || !ImGui::IsAnyItemActive()) {
            auto& actionMap = context.GetActionMap();
            if (actionMap.IsTriggered("Undo")) {
                Engine::Editor::CommandHistory::GetInstance().Undo();
            }
            if (actionMap.IsTriggered("Redo")) {
                Engine::Editor::CommandHistory::GetInstance().Redo();
            }
        }
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
