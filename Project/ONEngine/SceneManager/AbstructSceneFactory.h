#pragma once

/// std
#include <string>
#include <unordered_map>
#include <functional>

/// scene
#include "BaseScene.h"

/// ===================================================
/// シーンの生成クラス
/// ===================================================
class AbstructSceneFactory {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	using SceneCreator = std::function<BaseScene*()>;


	AbstructSceneFactory(const std::string& _startupSceneName);
	virtual ~AbstructSceneFactory() {}

	/// @brief シーンを生成する
	/// @param _sceneName 生成するシーンの名前
	/// @return シーンのポインタ
	BaseScene* CreateScene(const std::string& _sceneName);

	/// @brief シーンを生成する関数を登録
	/// @param _sceneName シーンの名前
	/// @param _creator シーンの生成関数
	bool RegisterSceneCreator(const std::string& _sceneName, SceneCreator _creator);

	/// @brief 初期化時のシーン名を取得
	/// @return 初期化時のシーン名
	const std::string& GetStartupSceneName() const { return startupSceneName_; }

	/// @brief シーンの名前の配列を取得
	/// @return シーンの名前の配列
	const std::vector<std::string>& GetSceneNames() const { return sceneNames_; }


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<std::string, SceneCreator> sceneCreator_;
	std::vector<std::string>                      sceneNames_;
	const std::string                             startupSceneName_;

};