#include "ImGuiGameWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "../ChildWindows/GameWindows/ImGuiProjectWindow.h"
#include "../ChildWindows/GameWindows/ImGuiGameSceneWindow.h"
#include "../ChildWindows/GameWindows/ImGuiSceneWindow.h"
#include "../ChildWindows/GameWindows/ImGuiInspectorWindow.h"
#include "../ChildWindows/GameWindows/ImGuiHierarchyWindow.h"
#include "../ChildWindows/GameWindows/ImGuiConsoleWindow.h"

ImGuiGameWindow::ImGuiGameWindow(
	EntityComponentSystem* _ecs,
	GraphicsResourceCollection* _resourceCollection,
	EditorManager* _editorManager,
	SceneManager* _sceneManager) {

	/// windowの設定
	imGuiFlags_ |= ImGuiWindowFlags_NoMove;
	imGuiFlags_ |= ImGuiWindowFlags_NoResize;
	imGuiFlags_ |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	/// ここでwindowを生成する
	IImGuiChildWindow* inspector = AddChild(std::make_unique<ImGuiInspectorWindow>(_ecs, _editorManager));
	AddChild(std::make_unique<ImGuiGameSceneWindow>(_resourceCollection));
	AddChild(std::make_unique<ImGuiNormalHierarchyWindow>("Hierarchy", _ecs, _editorManager, _sceneManager, static_cast<ImGuiInspectorWindow*>(inspector)));
	AddChild(std::make_unique<ImGuiHierarchyWindow>("DebugHierarchy", _ecs->GetECSGroup("Debug"), _editorManager, _sceneManager, static_cast<ImGuiInspectorWindow*>(inspector)));
	AddChild(std::make_unique<ImGuiSceneWindow>(_ecs, _resourceCollection, _sceneManager, static_cast<ImGuiInspectorWindow*>(inspector)));
	AddChild(std::make_unique<ImGuiProjectWindow>(_resourceCollection, _editorManager));
	AddChild(std::make_unique<ImGuiConsoleWindow>());
}



void ImGuiGameWindow::ImGuiFunc() {

	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowSize(ImVec2(EngineConfig::kWindowSize.x, EngineConfig::kWindowSize.y));
	if (!ImGui::Begin("Game", nullptr, imGuiFlags_)) {
		ImGui::End();
		return;
	}

	ImGuiID dockspaceID = ImGui::GetID("GameDockingSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));

	UpdateChildren();

	ImGui::End();
}