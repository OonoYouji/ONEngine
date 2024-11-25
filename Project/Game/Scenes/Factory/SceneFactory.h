#pragma once

/// std
#include <string>
#include <unordered_map>

/// base class
#include "../Base/AbstructSceneFactory.h"


class SceneFactory : public AbstructSceneFactory {
public:
	SceneFactory();
	~SceneFactory();

	BaseScene* CreateScene(const std::string& _sceneName) override;

private:
	//std::unordered_map<std::string, >;
};