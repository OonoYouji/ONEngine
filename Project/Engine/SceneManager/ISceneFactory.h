#pragma once

/// std
#include <memory>
#include <functional>
#include <string>
#include <type_traits>
#include <iostream>

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
	/// シーンをファクトリーに登録する
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="_sceneName"></param>
	template <typename T, typename std::enable_if<std::is_base_of<IScene, T>::value>::type* = nullptr>
	void RegisterScene(const std::string& _sceneName);

private:

	std::unordered_map<std::string, SceneCreator> sceneCreatorMap_;

};



template <typename T, typename std::enable_if<std::is_base_of<IScene, T>::value>::type*>
inline void ISceneFactory::RegisterScene(const std::string& _sceneName) {
	sceneCreatorMap_[_sceneName] = []() { return std::make_unique<T>(); };
}
