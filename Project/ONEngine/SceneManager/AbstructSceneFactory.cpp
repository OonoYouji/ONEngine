#include "AbstructSceneFactory.h"

AbstructSceneFactory::AbstructSceneFactory(const std::string& _startupSceneName) :
	startupSceneName_(_startupSceneName) {

}



BaseScene* AbstructSceneFactory::CreateScene(const std::string& _sceneName) {
	return sceneCreator_.at(_sceneName)();
}

bool AbstructSceneFactory::RegisterSceneCreator(const std::string& _sceneName, SceneCreator _creator) {

	/// すでにregisterされているかチェック
	auto itr = sceneCreator_.find(_sceneName);

	/// なかったら登録
	if(itr == sceneCreator_.end()) {
		sceneCreator_[_sceneName] = _creator;
		sceneNames_.push_back(_sceneName);
		return true;
	}

	/// あったのでなにもしない
	return false;
}
