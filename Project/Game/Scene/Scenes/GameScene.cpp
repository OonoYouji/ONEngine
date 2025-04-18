#include "GameScene.h"

/// std
#include <numbers>

/// engine
#include "Engine/ECS/Entity/Grid/Grid.h"
#include "Engine/ECS/Entity/Camera/Camera.h"

/// user
#include "Game/Objects/Terrain/Terrain.h"
#include "Game/Entity/Player/Player.h"

GameScene::GameScene() {
	loadResourcePaths_ = {
		"Assets/Models/primitive/cube.obj",
		"Assets/Models/primitive/sphere.obj",
		"Assets/Models/primitive/cylinder.obj",
		"Assets/Models/multiMeshTest/test.obj",
		"Assets/Models/entity/player.obj",
	};

	unloadResourcePaths_ = {
		"Assets/Models/cube/cube.obj"
	};
}

GameScene::~GameScene() {}


void GameScene::Initialize() {

#ifdef _DEBUG
	pEntityComponentSystem_->GenerateEntity<Grid>();
#endif // _DEBUG
	
	//pEntityComponentSystem_->GenerateEntity<GameController>();
	Player* player = pEntityComponentSystem_->GenerateEntity<Player>();
	Camera* camera = pEntityComponentSystem_->GenerateCamera();
	camera->SetParent(player);
	camera->SetPosition(Vector3(0, 1.8f, -2.5f));
	camera->SetRotateX(std::numbers::pi_v<float> * 0.1f);

	pEntityComponentSystem_->SetMainCamera(camera);
	pEntityComponentSystem_->GenerateEntity<Terrain>();

}

void GameScene::Update() {


}

