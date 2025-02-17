#include "GameScene.h"

#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/Component/RendererComponents/Primitive/Line2DRenderer.h"

#include "Engine/Entity/Demo/DemoEntity.h"


GameScene::GameScene() {
	loadResourcePaths_ = {
		"Assets/Models/primitive/cube.obj",
		"Assets/Models/primitive/sphere.obj",
		"Assets/Models/primitive/cylinder.obj",
		"Assets/Models/multiMeshTest/test.obj",
		"Assets/Textures/uvChecker.png"
	};

	unloadResourcePaths_ = {
		"Assets/Models/cube/cube.obj"
	};
}

GameScene::~GameScene() {}


void GameScene::Initialize() {
	Camera* camera = entityCollection_->GenerateCamera();
	Transform* transform =camera->GetTransform();
	transform->SetPosition(Vector3(0.0f, 0.0f, -5.0f));
	transform->SetRotate(Vector3(0.0f, 0.0f, 0.0f));
	
	entityCollection_->GenerateEntity<DemoEntity>();

}

void GameScene::Update() {

}

