#include "ImGuiGameWindow.h"

using namespace ONEngine;

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
#include "../ChildWindows/GameWindows/ImGuiTexturePreviewWindow.h"

ImGuiGameWindow::ImGuiGameWindow(
	DxManager* _dxManager,
	EntityComponentSystem* _ecs, AssetCollection* _assetCollection,
	EditorManager* _editorManager, SceneManager* _sceneManager) {

	/// windowの設定
	imGuiFlags_ |= ImGuiWindowFlags_NoMove;
	imGuiFlags_ |= ImGuiWindowFlags_NoResize;
	imGuiFlags_ |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	/// 子windowの追加
	ImGuiInspectorWindow* inspector = static_cast<ImGuiInspectorWindow*>(AddChild(std::make_unique<ImGuiInspectorWindow>("Inspector##Game", _dxManager, _ecs, _assetCollection, _editorManager)));
	AddChild(std::make_unique<ImGuiGameSceneWindow>(_assetCollection));
	AddChild(std::make_unique<ImGuiNormalHierarchyWindow>("Hierarchy", _ecs, _editorManager, _sceneManager));
	AddChild(std::make_unique<ImGuiHierarchyWindow>("DebugHierarchy", _ecs->GetECSGroup("Debug"), _editorManager, _sceneManager));
	AddChild(std::make_unique<ImGuiSceneWindow>(_ecs, _assetCollection, _sceneManager, inspector));
	AddChild(std::make_unique<ImGuiProjectWindow>(_assetCollection, _editorManager));
	AddChild(std::make_unique<ImGuiConsoleWindow>());
	AddChild(std::make_unique<ImGuiTexturePreviewWindow>(_assetCollection));
}



void ImGuiGameWindow::ShowImGui() {

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
