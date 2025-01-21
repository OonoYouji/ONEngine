#pragma once

/// std
#include <memory>
#include <vector>
#include <string>

/// engine
#include "IScene.h"
#include "ISceneFactory.h"


/// ===================================================
/// シーンの管理を行うクラス
/// ===================================================
class SceneManager final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	SceneManager();
	~SceneManager();

	/// @brief このクラスの初期化
	void Initialize();

	/// @brief シーンの更新
	void Update();

	/// @brief 次のシーンを設定する
	/// @param _sceneName 次のシーンの名前
	void SetNextScene(const std::string& _sceneName);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<IScene> currentScene_ = nullptr;
	std::unique_ptr<IScene> nextScene_    = nullptr;

	std::unique_ptr<ISceneFactory> sceneFactory_ = nullptr;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief シーンファクトリーのクラスを登録
	/// @param _sceneFactory シーンファクトリーのインスタンス
	void SetSceneFactory(std::unique_ptr<ISceneFactory>& _sceneFactory);

};

