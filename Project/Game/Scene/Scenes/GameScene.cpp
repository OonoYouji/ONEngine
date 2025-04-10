#include "GameScene.h"

/// std
#include <numbers>

/// engine
#include "Engine/Entity/Grid/Grid.h"

/// user
#include "Game/Entity/GameController/GameController.h"
#include "Game/Objects/Terrain/Terrain.h"


GameScene::GameScene() {
	loadResourcePaths_ = {
		"Assets/Models/primitive/cube.obj",
		"Assets/Models/primitive/sphere.obj",
		"Assets/Models/primitive/cylinder.obj",
		"Assets/Models/multiMeshTest/test.obj",
		"Assets/Models/entity/player.obj",
		"Assets/Textures/white.png",
		"Assets/Textures/uvChecker.png",
	};

	unloadResourcePaths_ = {
		"Assets/Models/cube/cube.obj"
	};
}

GameScene::~GameScene() {}


void GameScene::Initialize() {

#ifdef _DEBUG
	entityCollection_->GenerateEntity<Grid>();
#endif // _DEBUG
	
	//entityCollection_->GenerateEntity<GameController>();
	entityCollection_->GenerateEntity<Terrain>();

}

void GameScene::Update() {


}

