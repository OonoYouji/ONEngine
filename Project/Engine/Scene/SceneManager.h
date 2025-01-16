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

	void Initialize();
	void Update();
	void Draw();

	/// <summary>
	/// 次のシーンを設定する
	/// </summary>
	/// <param name="_sceneName"></param>
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

	/// <summary>
	/// SceneFactoryを設定する
	/// </summary>
	/// <param name="_sceneFactory"></param>
	void SetSceneFactory(std::unique_ptr<ISceneFactory>& _sceneFactory);

};

