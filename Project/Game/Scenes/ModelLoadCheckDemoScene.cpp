#include "ModelLoadCheckDemoScene.h"

/// user
#include "Objects/ModelLoadChecker/ModelLoadChecker.h"

void ModelLoadCheckDemoScene::Initialize() {

	ModelLoadChecker* modelLoadChecker = new ModelLoadChecker();
	modelLoadChecker->Initialize();

}

void ModelLoadCheckDemoScene::Update() {

}
