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

	void SaveScene(const std::string& _name, class ECSGroup* _ecsGroup);
	void SaveCurrentScene();
	void SaveCurrentSceneTemporary();

	void LoadScene(const std::string& _sceneName);
	void ReloadScene(bool _isTemporary);

	SceneIO* GetSceneIO();

	std::string LastOpenSceneName();

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

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

	const std::string& GetCurrentSceneName() const;
};



namespace MONO_INTERNAL_METHOD {

	void InternalLoadScene(MonoString* _sceneName);
}