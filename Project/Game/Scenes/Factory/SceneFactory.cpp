#include "SceneFactory.h"

#include "../DemoScene.h"
#include "../PointLightDemoScene.h"

SceneFactory::SceneFactory(const std::string& _startupScene) : AbstructSceneFactory("SpotLightDemoScene") {

	RegisterSceneCreator("SpotLightDemoScene", []() { return new DemoScene(); });
	RegisterSceneCreator("PointLightDemoScene", []() { return new PointLightDemoScene(); });
}

SceneFactory::~SceneFactory() {}


