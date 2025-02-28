#include "GameScene.h"

/// std
#include <numbers>

/// engine
#include "Engine/Entity/Grid/Grid.h"

/// user
#include "Game/Entity/Player/Player.h"


GameScene::GameScene() {
	loadResourcePaths_ = {
		"Assets/Models/primitive/cube.obj",
		"Assets/Models/primitive/sphere.obj",
		"Assets/Models/primitive/cylinder.obj",
		"Assets/Models/multiMeshTest/test.obj",
		"Assets/Models/entity/player.obj",
		"Assets/Textures/white.png",
		"Assets/Textures/uvChecker.png",
		"Assets/Textures/player.png",
		"Assets/Textures/sword.png",
		"Assets/Textures/enemy.png",
	};

	unloadResourcePaths_ = {
		"Assets/Models/cube/cube.obj"
	};
}

GameScene::~GameScene() {}


void GameScene::Initialize() {
	entityCollection_->GenerateEntity<Grid>();
	
	Camera* camera = entityCollection_->GenerateCamera();
	camera->SetPosition(Vector3(0.0f, 20.0f, -25.0f));
	camera->SetRotate(Vector3(std::numbers::pi_v<float> / 5.0f, 0.0f, 0.0f));

	entityCollection_->GenerateEntity<Player>();


}

void GameScene::Update() {


}

