#include "GameScene.h"

/// engine
#include "Engine/Component/RendererComponents/Sprite/SpriteRenderer.h"
#include "Engine/Component/RendererComponents/Primitive/Line2DRenderer.h"
#include "Engine/Entity/Demo/DemoEntity.h"
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
		"Assets/Textures/gradation.png",
	};

	unloadResourcePaths_ = {
		"Assets/Models/cube/cube.obj"
	};
}

GameScene::~GameScene() {}


void GameScene::Initialize() {
	entityCollection_->GenerateEntity<Grid2D>();
	entityCollection_->GenerateCamera2D();

	entityCollection_->GenerateEntity<Player>();

}

void GameScene::Update() {

}

