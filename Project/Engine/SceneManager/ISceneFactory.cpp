#include "ISceneFactory.h"

std::unique_ptr<IScene> ISceneFactory::CreateScene(const std::string& _sceneName) {
	auto itr = sceneCreatorMap_.find(_sceneName);
	if(itr == sceneCreatorMap_.end()) {
		return nullptr;
	}

	return std::move(itr->second());
}
