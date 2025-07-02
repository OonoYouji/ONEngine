#include "IScene.h"

/// engine
#include "SceneManager.h"

IScene::IScene(EntityComponentSystem* _entityComponentSystem, const std::string& _name)
	: pEntityComponentSystem_(_entityComponentSystem), sceneName_(_name) {}

void IScene::SetSceneManagerPtr(SceneManager* _sceneManager) {
	pSceneManager_ = _sceneManager;
}

void IScene::SetEntityComponentSystem(EntityComponentSystem* _pEntityComponentSystem) {
	pEntityComponentSystem_ = _pEntityComponentSystem;
}

const std::vector<std::string>& IScene::GetLoadResourcePaths() const {
	return loadResourcePaths_;
}

const std::vector<std::string>& IScene::GetUnloadResourcePaths() const {
	return unloadResourcePaths_;
}

const std::string& IScene::GetSceneName() const {
	return sceneName_;
}
