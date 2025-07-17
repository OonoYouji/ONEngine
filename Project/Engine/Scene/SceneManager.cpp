#include "SceneManager.h"

/// std
#include <numbers>

/// engine
#include "Scene/Factory/SceneFactory.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"


SceneManager::SceneManager(EntityComponentSystem* entityComponentSystem_)
	: pEntityComponentSystem_(entityComponentSystem_) {}
SceneManager::~SceneManager() {}


void SceneManager::Initialize(GraphicsResourceCollection* _graphicsResourceCollection) {

	pGraphicsResourceCollection_ = _graphicsResourceCollection;

	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneFactory_->Initialize();

	sceneIO_ = std::make_unique<SceneIO>(pEntityComponentSystem_);


	SetNextScene(sceneFactory_->GetStartupSceneName());
	MoveNextToCurrentScene(false);


	/// カメラを設定する
	auto cameras = pEntityComponentSystem_->FindEntities<Camera>();
	for (auto& camera : cameras) {
		if (camera->GetName() == "DebugCamera") {
			continue;
		}

		if (camera->GetCameraType() == static_cast<int>(CameraType::Type3D)) {
			pEntityComponentSystem_->SetMainCamera(camera);
			break;
		}

		if (camera->GetCameraType() == static_cast<int>(CameraType::Type2D)) {
			pEntityComponentSystem_->SetMainCamera2D(camera);
			break;
		}
	}

}

void SceneManager::Update() {

	/// 次のシーンが設定されていたら、シーンを切り替える
	if (nextScene_ != nullptr) {
		MoveNextToCurrentScene(false);
	}

	/// 現在のシーンの更新処理
	currentScene_->Update();

}

void SceneManager::SetNextScene(const std::string& _sceneName) {
	nextScene_ = sceneFactory_->CreateScene(_sceneName, pEntityComponentSystem_);
}

void SceneManager::SaveCurrentScene() {
	if (currentScene_ == nullptr) {
		Console::Log("No current scene to save.");
		return;
	}

	sceneIO_->Output(currentScene_.get());
}

void SceneManager::SaveCurrentSceneTemporary() {
	if (currentScene_ == nullptr) {
		Console::Log("No current scene to save temporarily.");
		return;
	}

	sceneIO_->OutputTemporary(currentScene_.get());
}

void SceneManager::StartCurrentScene() {

}

void SceneManager::LoadScene(const std::string& _sceneName) {
	SetNextScene(_sceneName);
	if (nextScene_ == nullptr) {
		Console::Log("Failed to load scene: " + _sceneName);
		return;
	}

	MoveNextToCurrentScene(false);
}

void SceneManager::ReloadScene(bool _isTemporary) {
	if (currentScene_ == nullptr) {
		Console::LogError("No current scene to reload.");
		return;
	}
	/// 現在のシーンを再読み込み
	SetNextScene(currentScene_->GetSceneName());
	if (nextScene_ == nullptr) {
		Console::LogError("Failed to reload scene: " + currentScene_->GetSceneName());
		return;
	}
	MoveNextToCurrentScene(_isTemporary);
}

void SceneManager::MoveNextToCurrentScene(bool _isTemporary) {
	currentScene_ = std::move(nextScene_);

	pEntityComponentSystem_->RemoveEntityAll();

	/// resourceの読み込み、解放をここで行う
	pGraphicsResourceCollection_->UnloadResources(currentScene_->unloadResourcePaths_);
	pGraphicsResourceCollection_->LoadResources(currentScene_->loadResourcePaths_);

	/// sceneに必要な情報を渡して初期化
	if (_isTemporary) {
		sceneIO_->InputTemporary(currentScene_.get());
	} else {
		sceneIO_->Input(currentScene_.get());
	}

	currentScene_->SetEntityComponentSystem(pEntityComponentSystem_);
	currentScene_->SetSceneManagerPtr(this);
	currentScene_->Initialize();
	Time::ResetTime();

	nextScene_ = nullptr;
}


void SceneManager::SetSceneFactory(std::unique_ptr<ISceneFactory>& _sceneFactory) {
	sceneFactory_ = std::move(_sceneFactory);
}

const std::string& SceneManager::GetCurrentSceneName() const {
	if (currentScene_) {
		return currentScene_->GetSceneName();
	}
	return "empty";
}


