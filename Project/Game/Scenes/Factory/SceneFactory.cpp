#include "SceneFactory.h"


SceneFactory::SceneFactory() {}
SceneFactory::~SceneFactory() {}

BaseScene* SceneFactory::CreateScene(const std::string& _sceneName) {
	return nullptr;
}
