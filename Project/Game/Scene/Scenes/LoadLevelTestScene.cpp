#include "LoadLevelTestScene.h"

/// std
#include <fstream>
#include <numbers>

/// engine
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/ECS/Entity/Entities/EmptyEntity/EmptyEntity.h"
#include "Engine/ECS/Entity/Entities/Grid/Grid.h"
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"

LoadLevelTestScene::LoadLevelTestScene() {
	loadResourcePaths_ = {
		"./Assets/Models/primitive/cube.obj",
	};
}

void LoadLevelTestScene::Initialize() {

	pEntityComponentSystem_->RemoveEntityAll();
	Grid* grid = pEntityComponentSystem_->GenerateEntity<Grid>();
	pEntityComponentSystem_->AddDoNotDestroyEntity(grid);

	loadSceneCommand_ = std::make_unique<LoadSceneCommand>(pEntityComponentSystem_, "./Assets/Levels/scene.json");
	loadSceneCommand_->Execute();
}

void LoadLevelTestScene::Update() {

}
