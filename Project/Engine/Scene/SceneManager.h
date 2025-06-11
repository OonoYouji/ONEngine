#pragma once

/// std
#include <memory>
#include <vector>
#include <string>

/// engine
#include "IScene.h"
#include "ISceneFactory.h"
#include "Loader/SceneIO.h"


/// ===================================================
/// シーンの管理を行うクラス
/// ===================================================
class SceneManager final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	SceneManager(class EntityComponentSystem* _entityCompnentSystem);
	~SceneManager();

	/// @brief このクラスの初期化
	void Initialize(class GraphicsResourceCollection* _graphicsResourceCollection);

	/// @brief シーンの更新
	void Update();

	/// @brief 次のシーンを設定する
	/// @param _sceneName 次のシーンの名前
	void SetNextScene(const std::string& _sceneName);

	void SaveCurrentScene();

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	void MoveNextToCurrentScene();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class EntityComponentSystem*      pEntityComponentSystem_      = nullptr;
	class GraphicsResourceCollection* pGraphicsResourceCollection_ = nullptr;

	std::unique_ptr<IScene>           currentScene_               = nullptr;
	std::unique_ptr<IScene>           nextScene_                  = nullptr;

	std::unique_ptr<ISceneFactory>    sceneFactory_               = nullptr;

	std::unique_ptr<SceneIO>          sceneIO_ = nullptr;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief シーンファクトリーのクラスを登録
	/// @param _sceneFactory シーンファクトリーのインスタンス
	void SetSceneFactory(std::unique_ptr<ISceneFactory>& _sceneFactory);

};

