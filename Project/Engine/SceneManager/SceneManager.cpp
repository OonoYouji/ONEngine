#include "SceneManager.h"


SceneManager::SceneManager() {}
SceneManager::~SceneManager() {}


void SceneManager::Initialize() {

}

void SceneManager::Update() {

	/// 次のシーンが設定されていたら、シーンを切り替える
	if(nextScene_ != nullptr) {
		currentScene_ = std::move(nextScene_);
		currentScene_->Initialize();
		nextScene_ = nullptr;
	}

	/// 現在のシーンの更新処理
	currentScene_->Update();

}

void SceneManager::Render() {
	currentScene_->Render();
}


void SceneManager::SetNextScene(const std::string* _sceneName) {
	
}


void SceneManager::SetSceneFactory(std::unique_ptr<ISceneFactory>& _sceneFactory) {
	sceneFactory_ = std::move(_sceneFactory);
}


