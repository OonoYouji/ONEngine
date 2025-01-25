#include "SceneFactory.h"

#include "../DemoScene.h"

SceneFactory::SceneFactory(const std::string& _startupScene) : AbstructSceneFactory("DemoScene") {

	RegisterSceneCreator("DemoScene", []() { return new DemoScene(); });
}

SceneFactory::~SceneFactory() {}


