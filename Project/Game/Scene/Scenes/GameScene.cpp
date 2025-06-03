#include "GameScene.h"

/// std
#include <numbers>

/// engine
#include "Engine/ECS/Entity/Demo/DemoEntity.h"
#include "Engine/ECS/Entity/Grid/Grid.h"
#include "Engine/ECS/Entity/Camera/Camera.h"
#include "Engine/ECS/Entity/Light/DirectionalLightObject.h"
#include "Engine/ECS/Entity/Skybox/Skybox.h"

/// user
#include "Game/Objects/Terrain/Terrain.h"
#include "Game/Entity/Player/Player.h"
#include "Game/Objects/KeyItem/KeyItem.h"
#include "Game/Effects/PuzzleStandEffects/PuzzleClearEffect.h"

//#include "Game/Objects/Enemy.h"

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
		"Assets/Models/objects/PuzzleStand/PuzzleStand.obj",
		"Assets/Models/objects/PuzzleClearEffect/PuzzleClearEffectLaser.obj",
		"Assets/Models/objects/PuzzleClearEffect/CannonStand.obj",
		"Assets/Models/objects/PuzzleClearEffect/Cannon.obj",

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

	//pEntityComponentSystem_->GenerateEntity<Enemy>();
	pEntityComponentSystem_->GenerateEntity<DirectionalLightObject>();

	//pEntityComponentSystem_->GenerateEntity<GameController>();
	Player* player = pEntityComponentSystem_->GenerateEntity<Player>();
	Camera* camera = pEntityComponentSystem_->GenerateCamera();
	camera->SetParent(player);
	camera->SetPosition(Vector3(0, 1.8f, -2.5f));
	camera->SetRotateX(std::numbers::pi_v<float> *0.1f);
	player->SetCamera(camera);

	Skybox* skybox = pEntityComponentSystem_->GenerateEntity<Skybox>();
	skybox->SetScale(Vector3::kOne * 500.0f);

	pEntityComponentSystem_->SetMainCamera(camera);
	KeyItem* keyItem = pEntityComponentSystem_->GenerateEntity<KeyItem>();
	keyItem->SetPosition(Vector3(-20, 0, 0));
	keyItem->UpdateTransform();

	terrainEditor_ = std::make_unique<TerrainEditor>(
		pEntityComponentSystem_->GenerateEntity<Terrain>(),
		pEntityComponentSystem_
	);
	terrainEditor_->Initialize();

	pEntityComponentSystem_->GenerateEntity<PuzzleClearEffect>();

}

void GameScene::Update() {
#ifdef _DEBUG
	terrainEditor_->Update();
#endif // _DEBUG

}

