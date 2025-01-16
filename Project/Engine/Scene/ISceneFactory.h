#pragma once

/// std
#include <memory>
#include <functional>
#include <string>
#include <type_traits>

/// engine
#include "IScene.h"


using SceneCreator = std::function<std::unique_ptr<IScene>()>;


/// ===================================================
/// シーンの生成を行うクラス
/// ===================================================
class ISceneFactory {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================
	
	virtual ~ISceneFactory() = default;

	/// <summary>
	/// 初期化の純粋仮想関数
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// シーンをファクトリーに登録する
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="_sceneName"></param>
	template <typename T, typename std::enable_if<std::is_base_of<IScene, T>::value>::type* = nullptr>
	void RegisterScene(const std::string& _sceneName);

	/// <summary>
	/// シーンの生成を行う
	/// </summary>
	/// <param name="_sceneName"> 登録したシーンの名前 </param>
	/// <returns> return : 成功したら生成したオブジェクト、 失敗したらnullptr</returns>
	std::unique_ptr<IScene> CreateScene(const std::string& _sceneName);

private:
	
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<std::string, SceneCreator> sceneCreatorMap_;

	std::string startupSceneName_; /// 初期化時のシーンの名前


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// 初期化時のシーン名を設定する
	/// </summary>
	/// <param name="_sceneName"></param>
	void SetStartupSceneName(const std::string& _sceneName);


};



template <typename T, typename std::enable_if<std::is_base_of<IScene, T>::value>::type*>
inline void ISceneFactory::RegisterScene(const std::string& _sceneName) {
	sceneCreatorMap_[_sceneName] = []() { return std::make_unique<T>(); };
}
