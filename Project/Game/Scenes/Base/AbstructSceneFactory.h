#pragma once

/// std
#include <string>

/// scene
#include "BaseScene.h"

class AbstructSceneFactory {
public:

	virtual ~AbstructSceneFactory() {}

	virtual BaseScene* CreateScene(const std::string& _sceneName) = 0;
};