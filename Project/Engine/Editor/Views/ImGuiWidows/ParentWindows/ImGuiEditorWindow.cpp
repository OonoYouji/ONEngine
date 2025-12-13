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

using namespace Editor;

ImGuiEditorWindow::ImGuiEditorWindow(
	ONEngine::DxManager* _dxm, ONEngine::EntityComponentSystem* _ecs, ONEngine::AssetCollection* _assetCollection, EditorManager* _editorManager, ONEngine::SceneManager* _sceneManager) {
	imGuiFlags_ |= ImGuiWindowFlags_NoMove;
	imGuiFlags_ |= ImGuiWindowFlags_NoResize;
	imGuiFlags_ |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	/// 子windowの追加
	InspectorWindow* inspector = static_cast<InspectorWindow*>(
		AddView(std::make_unique<InspectorWindow>("Inspector##Prefab", _dxm, _ecs, _assetCollection, _editorManager)));

	AddView(std::make_unique<PrefabFileWindow>(_ecs, _assetCollection, inspector));
	AddView(std::make_unique<PrefabViewWindow>(_ecs, _assetCollection));
	AddView(std::make_unique<HierarchyWindow>("Hierarchy##Prefab", _ecs->GetECSGroup("Debug"), _editorManager, _sceneManager));
	ProjectWindow* project = static_cast<ProjectWindow*>(
		AddView(std::make_unique<ProjectWindow>(_assetCollection, _editorManager)));

	project->SetWindowName("Prefab Project");

}

void ImGuiEditorWindow::ShowImGui() {

	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(ONEngine::EngineConfig::kWindowSize.x, ONEngine::EngineConfig::kWindowSize.y), ImGuiCond_Appearing);
	if (!ImGui::Begin("Prefab", nullptr, imGuiFlags_)) {
		ImGui::End();
		return;
	}

	ImGuiID dockspaceID = ImGui::GetID("EditorDockingSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));

	UpdateViews();

	ImGui::End();
}
