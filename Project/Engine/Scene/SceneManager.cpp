#include "SceneManager.h"

using namespace ONEngine;

/// std
#include <numbers>
#include <fstream>

/// external
#include <nlohmann/json.hpp>

/// engine
//#include "Scene/Factory/SceneFactory.h"
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Utility/Tools/Log.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


namespace {
	/// @brief monoに登録する関数で使用するために
	SceneManager* gSceneManager = nullptr;
}

/**
 * @brief コンストラクタ。
 * @param entityComponentSystem_ ECS管理クラスへのポインタ
 */
SceneManager::SceneManager(EntityComponentSystem* entityComponentSystem_)
	: pEcs_(entityComponentSystem_) {
}
/**
 * @brief デストラクタ。最後に開いていたシーン名をディスクに保存します。
 */
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


/**
 * @brief 初期化処理。SceneIOの生成および初期ロードを実行します。
 * @param _assetCollection アセットコレクションへのポインタ
 */
void SceneManager::Initialize(Asset::AssetCollection* _assetCollection) {
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

/**
 * @brief 更新処理。次のシーンが要求されている場合は遷移を実行します。
 */
void SceneManager::Update() {
	/// 次のシーンが設定されていたらシーンを切り替える
	if (nextScene_.size()) {
		MoveNextToCurrentScene(false);
	}
}

/**
 * @brief 次のフレームで遷移するシーン名をセットします。
 * @param _sceneName 遷移先シーン名
 */
void SceneManager::SetNextScene(const std::string& _sceneName) {
	nextScene_ = _sceneName;
}

/**
 * @brief 指定したECSグループの情報を指定したシーン名でシリアライズ保存します。
 * @param _name シーン名
 * @param _ecsGroup 保存対象のグループ
 */
void SceneManager::SaveScene(const std::string& _name, ECSGroup* _ecsGroup) {
	if (_name.empty() || !_ecsGroup) {
		Console::LogError("Invalid scene name or ECS group.");
		return;
	}

	sceneIO_->Output(_name, _ecsGroup);
	SetDirty(false);
}

/**
 * @brief 現在のアクティブなシーンの状況をファイルに保存します。
 */
void SceneManager::SaveCurrentScene() {
	if (currentScene_.empty()) {
		Console::LogError("No current scene to save.");
		return;
	}

	sceneIO_->Output(currentScene_, pEcs_->GetCurrentGroup());
	SetDirty(false);
}

/**
 * @brief 現在のシーンのエンティティ情報をメモリ上に一時保存（キャッシュ）します。
 */
void SceneManager::SaveCurrentSceneTemporary() {
	sceneIO_->OutputTemporary(pEcs_->GetCurrentGroup());
}

/**
 * @brief 新しいシーンを即時ロード（遷移）します。
 * @param _sceneName ロードするシーン名
 */
void SceneManager::LoadScene(const std::string& _sceneName) {
	SetNextScene(_sceneName);
	if (nextScene_.empty()) {
		Console::LogError("Failed to load scene: " + _sceneName);
		return;
	}

	MoveNextToCurrentScene(false);
}

/**
 * @brief 現在のシーンをリロードします。
 * @param _isTemporary 一時メモリ（キャッシュ）からロードする場合はtrue
 */
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

/**
 * @brief SceneIOオブジェクトのポインタを取得します。
 * @return SceneIOのポインタ
 */
SceneIO* SceneManager::GetSceneIO() {
	return sceneIO_.get();
}

/**
 * @brief 最後に開いていたシーン名をJSONファイルから読み込んで取得します。
 * @return 最後に開いたシーン名
 */
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

/**
 * @brief シーン変更フラグ（Dirty）を設定します。シーン編集が発生した際に呼び出されます。
 */
void SceneManager::MarkDirty() {
	isDirty_ = true;
}

/**
 * @brief シーンが変更されているか（保存が必要な状態か）を判定します。
 * @return 変更されている場合はtrue
 */
bool SceneManager::IsDirty() const {
	return isDirty_;
}

/**
 * @brief シーンの変更フラグを設定します。
 * @param _isDirty 設定するフラグの値
 */
void SceneManager::SetDirty(bool _isDirty) {
	isDirty_ = _isDirty;
}

/**
 * @brief 実際のシーン移行処理。現在のECSグループを破棄し、新しいグループを作成してJSONからロードします。
 * @param _isTemporary 一時的なメモリデータから復元する場合はtrue
 */
void SceneManager::MoveNextToCurrentScene(bool _isTemporary) {
	/// GPUの処理が終わるまで待つ（リソース破棄中のアクセスを防ぐ）
	pEcs_->GetDxManager()->GetDxCommand()->WaitForGpuComplete();

	ECSGroup* prevSceneGroup = pEcs_->GetCurrentGroup();
	if (prevSceneGroup) {
		prevSceneGroup->RemoveEntityAll();
	}

	currentScene_ = std::move(nextScene_);
	nextScene_.clear();

	ECSGroup* nextSceneGroup = pEcs_->AddECSGroup(GetCurrentSceneName());
	const std::string& sceneName = nextSceneGroup->GetGroupName();

	pEcs_->SetCurrentGroupName(sceneName);

	/// sceneに必要な情報を渡して初期化
	if (_isTemporary) {
		sceneIO_->InputTemporary(nextSceneGroup);
		return;
	}

	sceneIO_->Input(sceneName, nextSceneGroup);

	SetDirty(false);

	Time::ResetTime();
}


/**
 * @brief 現在アクティブなシーン名を取得します。
 * @return 現在のシーン名
 */
const std::string& SceneManager::GetCurrentSceneName() const {
	return currentScene_;
}



/**
 * @brief C#側（Mono内部コール）から呼び出され、指定シーンのロード要求を行います。
 * @param _sceneName シーン名文字列
 */
void MonoInternalMethods::InternalLoadScene(MonoString* _sceneName) {
	char* cstr = mono_string_to_utf8(_sceneName);
	if (gSceneManager) {
		gSceneManager->LoadScene(cstr);
	}

	mono_free(cstr);
}
