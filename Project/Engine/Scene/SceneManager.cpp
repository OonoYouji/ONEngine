#include "SceneManager.h"

/// std
#include <numbers>

/// engine
#include "Scene/Factory/SceneFactory.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


namespace {
	/// @brief monoに登録する関数で使用するために
	SceneManager* gSceneManager = nullptr;
}

SceneManager::SceneManager(EntityComponentSystem* entityComponentSystem_)
	: pECS_(entityComponentSystem_) {}
SceneManager::~SceneManager() {}


void SceneManager::Initialize(GraphicsResourceCollection* _graphicsResourceCollection) {
	gSceneManager = this;

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

void SceneManager::SaveScene(const std::string& _name, ECSGroup* _ecsGroup) {
	if (_name.empty() || !_ecsGroup) {
		Console::LogError("Invalid scene name or ECS group.");
		return;
	}

	sceneIO_->Output(_name, _ecsGroup);
}

void SceneManager::SaveCurrentScene() {
	if (currentScene_.empty()) {
		Console::LogError("No current scene to save.");
		return;
	}

	sceneIO_->Output(currentScene_, pECS_->GetCurrentGroup());
}

void SceneManager::SaveCurrentSceneTemporary() {
	if (currentScene_.empty()) {
		Console::LogError("No current scene to save temporarily.");
		return;
	}

	sceneIO_->OutputTemporary(currentScene_, pECS_->GetCurrentGroup());
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

SceneIO* SceneManager::GetSceneIO() {
	return sceneIO_.get();
}

void SceneManager::MoveNextToCurrentScene(bool _isTemporary) {
	ECSGroup* prevSceneGroup = pECS_->GetCurrentGroup();
	if (prevSceneGroup) {
		prevSceneGroup->RemoveEntityAll();
	}

	currentScene_ = std::move(nextScene_);

	ECSGroup* nextSceneGroup = pECS_->AddECSGroup(GetCurrentSceneName());
	const std::string& sceneName = nextSceneGroup->GetGroupName();

	pECS_->SetCurrentGroupName(sceneName);

	/// sceneに必要な情報を渡して初期化
	if (_isTemporary) {
		sceneIO_->InputTemporary(currentScene_, nextSceneGroup);
		return;
	}

	sceneIO_->Input(sceneName, nextSceneGroup);

	Time::ResetTime();
}


void SceneManager::SetSceneFactory(std::unique_ptr<ISceneFactory>& _sceneFactory) {
	sceneFactory_ = std::move(_sceneFactory);
}

const std::string& SceneManager::GetCurrentSceneName() const {
	return currentScene_;
}



void MONO_INTERNAL_METHOD::InternalLoadScene(MonoString* _sceneName) {
	char* cstr = mono_string_to_utf8(_sceneName);
	if (gSceneManager) {
		gSceneManager->LoadScene(cstr);
	}

	mono_free(cstr);
}
