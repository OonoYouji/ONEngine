#pragma once

/// std
#include <string>
#include <unordered_map>

/// base class
#include "SceneManager/AbstructSceneFactory.h"


class SceneFactory : public AbstructSceneFactory {
public:
	SceneFactory(const std::string& _startupScene);
	~SceneFactory();
};