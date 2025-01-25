#include "SceneFactory.h"

#include "../DemoScene.h"
#include "../PointLightDemoScene.h"
#include "../PhongLightDemoScene.h"
#include "../ModelLoadCheckDemoScene.h"

SceneFactory::SceneFactory(const std::string& _startupScene) : AbstructSceneFactory("PhongLightDemoScene") {

	RegisterSceneCreator("SpotLightDemoScene", []() { return new DemoScene(); });
	RegisterSceneCreator("PointLightDemoScene", []() { return new PointLightDemoScene(); });
	RegisterSceneCreator("PhongLightDemoScene", []() { return new PhongLightDemoScene(); });
	RegisterSceneCreator("ModelLoadCheckDemoScene", []() { return new ModelLoadCheckDemoScene(); });
}

SceneFactory::~SceneFactory() {}


