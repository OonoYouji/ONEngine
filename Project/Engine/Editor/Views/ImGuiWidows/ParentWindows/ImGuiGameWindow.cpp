#include "ImGuiGameWindow.h"


/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "../ChildWindows/GameWindows/ProjectWindow.h"
#include "../ChildWindows/GameWindows/GameSceneWindow.h"
#include "../ChildWindows/GameWindows/DebugSceneView.h"
#include "../ChildWindows/GameWindows/InspectorWindow.h"
#include "../ChildWindows/GameWindows/HierarchyWindow.h"
#include "../ChildWindows/GameWindows/ConsoleWindow.h"
#include "../ChildWindows/GameWindows/TexturePreviewWindow.h"

using namespace ONEngine;
using namespace Editor;

ImGuiGameWindow::ImGuiGameWindow(
	DxManager* _dxm,
	EntityComponentSystem* _ecs, AssetCollection* _assetCollection,
	EditorManager* _editorManager, SceneManager* _sceneManager) {

	/// windowの設定
	imGuiFlags_ |= ImGuiWindowFlags_NoMove;
	imGuiFlags_ |= ImGuiWindowFlags_NoResize;
	imGuiFlags_ |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	/// 子windowの追加
	InspectorWindow* inspector = static_cast<InspectorWindow*>(AddView(std::make_unique<InspectorWindow>("Inspector##Game", _dxm, _ecs, _assetCollection, _editorManager)));
	AddView(std::make_unique<GameSceneWindow>(_assetCollection));
	AddView(std::make_unique<ImGuiNormalHierarchyWindow>("Hierarchy", _ecs, _editorManager, _sceneManager));
	AddView(std::make_unique<HierarchyWindow>("DebugHierarchy", _ecs->GetECSGroup("Debug"), _editorManager, _sceneManager));
	AddView(std::make_unique<DebugSceneView>(_ecs, _assetCollection, _sceneManager, inspector));
	AddView(std::make_unique<ProjectWindow>(_assetCollection, _editorManager));
	AddView(std::make_unique<ConsoleWindow>());
	AddView(std::make_unique<TexturePreviewWindow>(_assetCollection));
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

	UpdateViews();

	ImGui::End();
}
