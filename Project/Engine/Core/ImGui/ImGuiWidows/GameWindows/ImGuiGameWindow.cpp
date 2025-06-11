#include "ImGuiGameWindow.h"

/// external
#include <imgui.h>

/// engine
#include "../GameWindows/ImGuiProjectWindow.h"
#include "../GameWindows/ImGuiGameSceneWindow.h"
#include "../GameWindows/ImGuiSceneWindow.h"
#include "../GameWindows/ImGuiInspectorWindow.h"
#include "../GameWindows/ImGuiHierarchyWindow.h"
#include "../GameWindows/ImGuiConsoleWindow.h"

ImGuiGameWindow::ImGuiGameWindow(
	EntityComponentSystem* _entityComponentSystem,
	GraphicsResourceCollection* _resourceCollection,
	EditorManager* _editorManager,
	SceneManager* _sceneManager) {

	/// windowの設定
	imGuiFlags_ |= ImGuiWindowFlags_NoMove;
	imGuiFlags_ |= ImGuiWindowFlags_NoResize;
	imGuiFlags_ |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	/// ここでwindowを生成する
	IImGuiChildWindow* inspector = AddChild(std::make_unique<ImGuiInspectorWindow>(_editorManager));
	AddChild(std::make_unique<ImGuiGameSceneWindow>(_resourceCollection));
	AddChild(std::make_unique<ImGuiSceneWindow>(_resourceCollection));
	AddChild(std::make_unique<ImGuiHierarchyWindow>(_entityComponentSystem, _editorManager, _sceneManager, static_cast<ImGuiInspectorWindow*>(inspector)));
	AddChild(std::make_unique<ImGuiProjectWindow>(_editorManager));
	AddChild(std::make_unique<ImGuiConsoleWindow>());
}



void ImGuiGameWindow::ImGuiFunc() {

	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(1920.0f, 1080.0f), ImGuiCond_Appearing);
	if (!ImGui::Begin("Game", nullptr, imGuiFlags_)) {
		ImGui::End();
		return;
	}

	ImGuiID dockspaceID = ImGui::GetID("GameDockingSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));

	UpdateChildren();

	ImGui::End();
}