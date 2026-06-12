#include "AITab.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "../Windows/Develop/BehaviorTreeEditorWindow.h"

using namespace ONEngine;
using namespace Editor;

AITab::AITab(
	DxManager* _dxm,
	EntityComponentSystem* _ecs,
	EditorManager* _editorManager, SceneManager* _sceneManager)
	: IEditorWindowContainer("AI") {

	(void)_dxm;
	(void)_editorManager;
	(void)_sceneManager;

	/// BehaviorTreeEditorWindowを追加
	AddView(std::make_unique<BehaviorTreeEditorWindow>("AI Behavior Tree", _ecs));
}
