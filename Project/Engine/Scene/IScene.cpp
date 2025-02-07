#include "IScene.h"

/// engine
#include "SceneManager.h"

void IScene::SetSceneManagerPtr(SceneManager* _sceneManager) {
	pSceneManager_ = _sceneManager;
}

void IScene::SetEntityCollectionPtr(EntityCollection* _entityCollection) {
	pEntityCollection_ = _entityCollection;
}
