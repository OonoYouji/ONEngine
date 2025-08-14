#include "SceneManager.h"

/// std
#include <numbers>

/// engine
#include "Scene/Factory/SceneFactory.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


SceneManager::SceneManager(EntityComponentSystem* entityComponentSystem_)
	: pECS_(entityComponentSystem_) {}
SceneManager::~SceneManager() {}


void SceneManager::Initialize(GraphicsResourceCollection* _graphicsResourceCollection) {
	pGraphicsResourceCollection_ = _graphicsResourceCollection;

	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneFactory_->Initialize();

	sceneIO_ = std::make_unique<SceneIO>(pECS_);

	SetNextScene(sceneFactory_->GetStartupSceneName());
	MoveNextToCurrentScene(false);

	pECS_->MainCameraSetting();
}

void SceneManager::Update() {
	/// 次のシーンが設定されていたら、シーンを切り替える
	if (nextScene_.size()) {
		MoveNextToCurrentScene(false);
	}
}

void SceneManager::SetNextScene(const std::string& _sceneName) {
	nextScene_ = _sceneName;
}

void SceneManager::SaveCurrentScene() {
	if (currentScene_.empty()) {
		Console::LogError("No current scene to save.");
		return;
	}

	sceneIO_->Output(currentScene_, pECS_->GetECSGroup(currentScene_));
}

void SceneManager::SaveCurrentSceneTemporary() {
	if (currentScene_.empty()) {
		Console::LogError("No current scene to save temporarily.");
		return;
	}

	sceneIO_->OutputTemporary(currentScene_, pECS_->GetECSGroup(currentScene_));
}

void SceneManager::LoadScene(const std::string& _sceneName) {
	SetNextScene(_sceneName);
	if (nextScene_.empty()) {
		Console::LogError("Failed to load scene: " + _sceneName);
		return;
	}

	MoveNextToCurrentScene(false);
}

void SceneManager::ReloadScene(bool _isTemporary) {
	if (currentScene_.empty()) {
		Console::LogError("No current scene to reload.");
		return;
	}
	/// 現在のシーンを再読み込み
	SetNextScene(currentScene_);
	if (nextScene_.empty()) {
		Console::LogError("Failed to reload scene: " + currentScene_);
		return;
	}
	MoveNextToCurrentScene(_isTemporary);
}

void SceneManager::MoveNextToCurrentScene(bool _isTemporary) {
	ECSGroup* gameSceneGroup = pECS_->GetECSGroup(currentScene_);
	if (gameSceneGroup) {
		gameSceneGroup->RemoveEntityAll();
	}

	currentScene_ = std::move(nextScene_);

	/// sceneに必要な情報を渡して初期化
	if (_isTemporary) {
		sceneIO_->InputTemporary(currentScene_);
	} else {
		sceneIO_->Input(currentScene_);
	}

	pECS_->SetCurrentGroupName(currentScene_);
	Time::ResetTime();
}


void SceneManager::SetSceneFactory(std::unique_ptr<ISceneFactory>& _sceneFactory) {
	sceneFactory_ = std::move(_sceneFactory);
}

const std::string& SceneManager::GetCurrentSceneName() const {
	return currentScene_;
}


