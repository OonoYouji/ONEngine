#include "PrefabTab.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "../Windows/Prefab/PrefabViewWindow.h"
#include "../Windows/Prefab/PrefabFileWindow.h"
#include "../Windows/Game/InspectorWindow.h"
#include "../Windows/Game/ProjectWindow.h"
#include "../Windows/Game/HierarchyWindow.h"

using namespace Editor;

PrefabTab::PrefabTab(
	ONEngine::DxManager* _dxm,
	ONEngine::EntityComponentSystem* _ecs,
	ONEngine::AssetCollection* _assetCollection,
	EditorManager* _editorManager,
	ONEngine::SceneManager* _sceneManager)
	: IEditorWindowContainer("Prefab") {

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
