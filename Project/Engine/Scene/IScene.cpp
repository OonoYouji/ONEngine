#include "IScene.h"

/// engine
#include "SceneManager.h"

void IScene::SetSceneManagerPtr(SceneManager* _sceneManager) {
	pSceneManager_ = _sceneManager;
}

void IScene::SetEntityComponentSystem(EntityComponentSystem* _pEntityComponentSystem) {
	pEntityComponentSystem_ = _pEntityComponentSystem;
}
