#pragma once

#include "../EditorViewCollection.h"

namespace ONEngine {
class DxManager;
class EntityComponentSystem;
class SceneManager;
}

namespace Editor {

class EditorManager;

class AITab : public IEditorWindowContainer {
public:
	AITab(
		ONEngine::DxManager* _dxm,
		ONEngine::EntityComponentSystem* _ecs,
		EditorManager* _editorManager,
		ONEngine::SceneManager* _sceneManager);
	~AITab() override = default;
};

} /// Editor
