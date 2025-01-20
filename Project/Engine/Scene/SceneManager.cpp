#include "SceneManager.h"

#include "Scene/Factory/SceneFactory.h"

SceneManager::SceneManager() {}
SceneManager::~SceneManager() {}


void SceneManager::Initialize() {

	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneFactory_->Initialize();

	currentScene_ = sceneFactory_->CreateScene("Game");

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

void SceneManager::Draw() {
	currentScene_->Draw();
}


void SceneManager::SetNextScene(const std::string& _sceneName) {
	nextScene_ = sceneFactory_->CreateScene(_sceneName);
}


void SceneManager::SetSceneFactory(std::unique_ptr<ISceneFactory>& _sceneFactory) {
	sceneFactory_ = std::move(_sceneFactory);
}


