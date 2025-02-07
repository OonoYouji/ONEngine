#include "SceneManager.h"

/// engine
#include "Scene/Factory/SceneFactory.h"
#include "Engine/Entity/Collection/EntityCollection.h"


SceneManager::SceneManager(EntityCollection* _entityCollection) 
	: pEntityCollection_(_entityCollection) {}
SceneManager::~SceneManager() {}


void SceneManager::Initialize() {

	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneFactory_->Initialize();

	currentScene_ = sceneFactory_->CreateScene("Game");

}

void SceneManager::Update() {

	/// 次のシーンが設定されていたら、シーンを切り替える
	if(nextScene_ != nullptr) {
		
	}

	/// 現在のシーンの更新処理
	currentScene_->Update();

}

void SceneManager::SetNextScene(const std::string& _sceneName) {
	nextScene_ = sceneFactory_->CreateScene(_sceneName);
}

void SceneManager::MoveNextToCurrentScene() {
	currentScene_ = std::move(nextScene_);
	
	currentScene_->SetEntityCollectionPtr(pEntityCollection_);
	currentScene_->SetSceneManagerPtr(this);
	currentScene_->Initialize();

	nextScene_ = nullptr;
}


void SceneManager::SetSceneFactory(std::unique_ptr<ISceneFactory>& _sceneFactory) {
	sceneFactory_ = std::move(_sceneFactory);
}


