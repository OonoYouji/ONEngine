#include "GameScene.h"

/// engine
#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/Component/RendererComponents/Primitive/Line2DRenderer.h"
#include "Engine/Entity/Demo/DemoEntity.h"
#include "Engine/Entity/Grid/Grid.h"

/// user
#include "Game/Entity/Block/BlockManager.h"
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
	entityCollection_->GenerateEntity<Grid>();

	Camera* camera = entityCollection_->GenerateCamera();
	Transform* transform =camera->GetTransform();
	transform->SetPosition(Vector3(0.0f, 0.0f, -5.0f));
	transform->SetRotate(Vector3(0.0f, 0.0f, 0.0f));
	
	entityCollection_->GenerateEntity<BlockManager>();
	entityCollection_->GenerateEntity<Player>();

}

void GameScene::Update() {

}

