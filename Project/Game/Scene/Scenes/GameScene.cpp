#include "GameScene.h"

/// engine
#include "Engine/Entity/Grid/Grid.h"

/// user
#include "Game/Entity/Player/Player.h"
#include "Game/Entity/Weapon/Weapon.h"


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
	entityCollection_->GenerateEntity<Grid2D>();
	Player*   player = entityCollection_->GenerateEntity<Player>();
	Camera2D* camera = entityCollection_->GenerateCamera2D();

	camera->SetParent(player);

	entityCollection_->GenerateEntity<Weapon>();

}

void GameScene::Update() {


}

