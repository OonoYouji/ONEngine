#include "ImGuiEditorWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "../ChildWindows/EditorWindows/ImGuiPrefabViewWindow.h"
#include "../ChildWindows/EditorWindows/ImGuiPrefabFileWindow.h"
#include "../ChildWindows/GameWindows/ImGuiInspectorWindow.h"
#include "../ChildWindows/GameWindows/ImGuiProjectWindow.h"
#include "../ChildWindows/GameWindows/ImGuiHierarchyWindow.h"

ImGuiEditorWindow::ImGuiEditorWindow(EntityComponentSystem* _ecs, AssetCollection* _assetCollection, EditorManager* _editorManager, SceneManager* _sceneManager) {
	imGuiFlags_ |= ImGuiWindowFlags_NoMove;
	imGuiFlags_ |= ImGuiWindowFlags_NoResize;
	imGuiFlags_ |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	/// 子windowの追加
	ImGuiInspectorWindow* inspector = static_cast<ImGuiInspectorWindow*>(
		AddChild(std::make_unique<ImGuiInspectorWindow>("Inspector##Prefab", _ecs, _assetCollection, _editorManager)));

	AddChild(std::make_unique<ImGuiPrefabFileWindow>(_ecs, _assetCollection, inspector));
	AddChild(std::make_unique<ImGuiPrefabViewWindow>(_ecs, _assetCollection));
	AddChild(std::make_unique<ImGuiHierarchyWindow>("Hierarchy##Prefab", _ecs->GetECSGroup("Debug"), _editorManager, _sceneManager));
	ImGuiProjectWindow* project = static_cast<ImGuiProjectWindow*>(
		AddChild(std::make_unique<ImGuiProjectWindow>(_assetCollection, _editorManager)));

	project->SetWindowName("Prefab Project");

}

void ImGuiEditorWindow::ShowImGui() {

	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(EngineConfig::kWindowSize.x, EngineConfig::kWindowSize.y), ImGuiCond_Appearing);
	if (!ImGui::Begin("Prefab", nullptr, imGuiFlags_)) {
		ImGui::End();
		return;
	}

	ImGuiID dockspaceID = ImGui::GetID("EditorDockingSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));

	UpdateChildren();

	ImGui::End();
}
