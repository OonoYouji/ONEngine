#include "SceneManager.h"

/// std
#include <numbers>
#include <fstream>

/// external
#include <nlohmann/json.hpp>

/// engine
//#include "Scene/Factory/SceneFactory.h"
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


namespace {
	/// @brief monoに登録する関数で使用するために
	SceneManager* gSceneManager = nullptr;
}

SceneManager::SceneManager(EntityComponentSystem* entityComponentSystem_)
	: pEcs_(entityComponentSystem_) {
}
SceneManager::~SceneManager() {
	/// 最後に開いていたシーンを保存
	if (!currentScene_.empty()) {
		nlohmann::json json;
		json["Scene"] = currentScene_;
		const std::string& filepath = "./Packages/Config/LastOpenScene.json";
		std::ofstream ofs(filepath);
		if (ofs.is_open()) {
			ofs << json.dump(4);
			ofs.close();
		}
	}
}


void SceneManager::Initialize(AssetCollection* _assetCollection) {
	gSceneManager = this;

	pAssetCollection_ = _assetCollection;

	sceneIO_ = std::make_unique<SceneIO>(pEcs_);

#ifdef DEBUG_MODE
	SetNextScene(LastOpenSceneName());
#else
	SetNextScene("TitleScene");
#endif

	MoveNextToCurrentScene(false);

	pEcs_->MainCameraSetting();
}

void SceneManager::Update() {
	/// 次のシーンが設定されていたらシーンを切り替える
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

	sceneIO_->Output(currentScene_, pEcs_->GetCurrentGroup());
}

void SceneManager::SaveCurrentSceneTemporary() {
	sceneIO_->OutputTemporary(pEcs_->GetCurrentGroup());
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

std::string SceneManager::LastOpenSceneName() {
	const std::string& filepath = "./Packages/Config/LastOpenScene.json";

	std::ifstream ifs(filepath);
	if (!ifs.is_open()) {
		return "";
	}

	nlohmann::json json;
	ifs >> json;

	ifs.close();
	if (json.contains("Scene") && json["Scene"].is_string()) {
		return json["Scene"];
	}

	return "";
}

void SceneManager::MoveNextToCurrentScene(bool _isTemporary) {
	ECSGroup* prevSceneGroup = pEcs_->GetCurrentGroup();
	if (prevSceneGroup) {
		prevSceneGroup->RemoveEntityAll();
	}

	currentScene_ = std::move(nextScene_);

	ECSGroup* nextSceneGroup = pEcs_->AddECSGroup(GetCurrentSceneName());
	const std::string& sceneName = nextSceneGroup->GetGroupName();

	pEcs_->SetCurrentGroupName(sceneName);

	/// sceneに必要な情報を渡して初期化
	if (_isTemporary) {
		sceneIO_->InputTemporary(nextSceneGroup);
		return;
	}

	sceneIO_->Input(sceneName, nextSceneGroup);

	Time::ResetTime();
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
