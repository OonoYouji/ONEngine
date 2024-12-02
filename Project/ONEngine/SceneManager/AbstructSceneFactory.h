#pragma once

/// std
#include <string>
#include <unordered_map>
#include <functional>

/// scene
#include "BaseScene.h"

class AbstructSceneFactory {
public:

	using SceneCreator = std::function<BaseScene*()>;


	AbstructSceneFactory(const std::string& _startupSceneName);
	virtual ~AbstructSceneFactory() {}

	/// <summary>
	/// シーンを作成する
	/// </summary>
	/// <param name="_sceneName"></param>
	/// <returns></returns>
	BaseScene* CreateScene(const std::string& _sceneName);


	/// <summary>
	/// シーンの生成関数を登録する
	/// </summary>
	/// <param name="_sceneName"></param>
	/// <param name="_createor"></param>
	/// <returns></returns>
	bool RegisterSceneCreator(const std::string& _sceneName, SceneCreator _createor);


	/// <summary>
	/// 初期化時のシーンの名前
	/// </summary>
	/// <returns></returns>
	const std::string& GetStartupSceneName() const { return startupSceneName_; }

private:
	std::unordered_map<std::string, SceneCreator> sceneCreator_;
	const std::string startupSceneName_;
};