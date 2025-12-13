#include "DevelopTab.h"


/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "../Windows/Develop/ProjectWindow.h"
#include "../Windows/Develop/GameSceneView.h"
#include "../Windows/Develop/DebugSceneView.h"
#include "../Windows/Develop/InspectorWindow.h"
#include "../Windows/Develop/HierarchyWindow.h"
#include "../Windows/Develop/ConsoleWindow.h"
#include "../Windows/Develop/TexturePreviewWindow.h"

using namespace ONEngine;
using namespace Editor;

DevelopTab::DevelopTab(
	DxManager* _dxm,
	EntityComponentSystem* _ecs, AssetCollection* _assetCollection,
	EditorManager* _editorManager, SceneManager* _sceneManager)
	: IEditorWindowContainer("Game") {

	/// 子windowの追加
	InspectorWindow* inspector = static_cast<InspectorWindow*>(AddView(std::make_unique<InspectorWindow>("Inspector##Game", _dxm, _ecs, _assetCollection, _editorManager)));
	AddView(std::make_unique<GameSceneView>(_assetCollection, "GameScene"));
	AddView(std::make_unique<ImGuiNormalHierarchyWindow>("Hierarchy", _ecs, _editorManager, _sceneManager));
	AddView(std::make_unique<HierarchyWindow>("DebugHierarchy", _ecs->GetECSGroup("Debug"), _editorManager, _sceneManager));
	AddView(std::make_unique<DebugSceneView>(_ecs, _assetCollection, _sceneManager, inspector));
	AddView(std::make_unique<ProjectWindow>(_assetCollection, _editorManager));
	AddView(std::make_unique<ConsoleWindow>());
	AddView(std::make_unique<TexturePreviewWindow>(_assetCollection));
}
