#include "PrefabTab.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "../ChildWindows/PrefabWindows/PrefabViewWindow.h"
#include "../ChildWindows/PrefabWindows/PrefabFileWindow.h"
#include "../ChildWindows/GameWindows/InspectorWindow.h"
#include "../ChildWindows/GameWindows/ProjectWindow.h"
#include "../ChildWindows/GameWindows/HierarchyWindow.h"

using namespace Editor;

PrefabTab::PrefabTab(
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

void PrefabTab::ShowImGui() {

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
