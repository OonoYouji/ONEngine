#pragma once

/// std
#include <memory>
#include <vector>
#include <string>

/// externals
#include <mono/jit/jit.h>

/// engine
#include "Loader/SceneIO.h"


/// ///////////////////////////////////////////////////
/// シーンの管理を行うクラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

class SceneManager final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SceneManager(class EntityComponentSystem* _entityCompnentSystem);
	~SceneManager();

	/// @brief このクラスの初期化
	void Initialize(class AssetCollection* _assetCollection);

	/// @brief シーンの更新
	void Update();

	/// @brief 次のシーンを設定する
	/// @param _sceneName 次のシーンの名前
	void SetNextScene(const std::string& _sceneName);

	/// @brief シーンの保存
	/// @param _name シーン名
	/// @param _ecsGroup 保存対象のECSGroup
	void SaveScene(const std::string& _name, class ECSGroup* _ecsGroup);

	/// @brief 現在のシーンを保存する
	void SaveCurrentScene();
	/// @brief 現在のシーンを一時的に保存する
	void SaveCurrentSceneTemporary();


	/// @brief シーンを読み込む
	/// @param _sceneName シーンの名前
	void LoadScene(const std::string& _sceneName);

	/// @brief 現在のシーンをリロードする
	/// @param _isTemporary 一時的なシーンかどうか
	void ReloadScene(bool _isTemporary);


	/// @brief シーンIOの取得
	SceneIO* GetSceneIO();

	/// @brief 最後に開いたシーン名を取得する
	std::string LastOpenSceneName();

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// @brief シーンを現在のシーンに移動する
	/// @param _isTemporary 一時的なシーンかどうか
	void MoveNextToCurrentScene(bool _isTemporary);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- other class ----- ///
	class EntityComponentSystem* pEcs_;
	class AssetCollection* pAssetCollection_;

	std::string currentScene_;
	std::string nextScene_;

	std::unique_ptr<SceneIO> sceneIO_;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief 現在のシーン名を取得する
	const std::string& GetCurrentSceneName() const;
};



namespace MONO_INTERNAL_METHOD {

	void InternalLoadScene(MonoString* _sceneName);
}

} /// ONEngine
