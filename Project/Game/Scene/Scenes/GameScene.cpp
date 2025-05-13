#include "GameScene.h"

/// std
#include <numbers>

/// engine
#include "Engine/ECS/Entity/Grid/Grid.h"
#include "Engine/ECS/Entity/Camera/Camera.h"
#include "Engine/ECS/Entity/Light/DirectionalLightObject.h"
#include "Engine/ECS/Entity/Skybox/Skybox.h"

/// user
#include "Game/Objects/Terrain/Terrain.h"
#include "Game/Entity/Player/Player.h"
#include "Game/Objects/KeyItem/KeyItem.h"

GameScene::GameScene() {
	loadResourcePaths_ = {
		"Assets/Models/primitive/cube.obj",
		"Assets/Models/primitive/sphere.obj",
		"Assets/Models/primitive/plane.obj",
		"Assets/Models/primitive/cylinder.obj",
		"Assets/Models/primitive/frontToPlane.obj",
		"Assets/Models/primitive/tube.obj",

		"Assets/Models/multiMeshTest/test.obj",
		"Assets/Models/entity/player.obj",
		"Assets/Models/objects/statue/statue.obj",
		"Assets/Models/objects/sideToPlane/sideToPlane.obj",

		"Assets/Textures/circle.png",
		"Assets/Textures/gradation.png",
		"Assets/Textures/ring.png",
		"Assets/Textures/gradationLine.png",
		"Assets/Textures/smoke.png",
	};

	//unloadResourcePaths_ = {
	//	"Assets/Models/cube/cube.obj"
	//};
}

GameScene::~GameScene() {}


void GameScene::Initialize() {

#ifdef _DEBUG
	//pEntityComponentSystem_->GenerateEntity<Grid>();
#endif // _DEBUG
	pEntityComponentSystem_->GenerateEntity<DirectionalLightObject>();

	//pEntityComponentSystem_->GenerateEntity<GameController>();
	Player* player = pEntityComponentSystem_->GenerateEntity<Player>();
	Camera* camera = pEntityComponentSystem_->GenerateCamera();
	camera->SetParent(player);
	camera->SetPosition(Vector3(0, 1.8f, -2.5f));
	camera->SetRotateX(std::numbers::pi_v<float> *0.1f);
	player->SetCamera(camera);

	pEntityComponentSystem_->GenerateEntity<Skybox>();
	pEntityComponentSystem_->SetMainCamera(camera);
	//pEntityComponentSystem_->GenerateEntity<KeyItem>();

	terrainEditor_ = std::make_unique<TerrainEditor>(
		pEntityComponentSystem_->GenerateEntity<Terrain>(),
		pEntityComponentSystem_
	);
	terrainEditor_->Initialize();
}

void GameScene::Update() {
#ifdef _DEBUG
	terrainEditor_->Update();
#endif // _DEBUG

}

