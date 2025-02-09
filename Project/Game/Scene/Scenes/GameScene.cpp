#include "GameScene.h"

#include "Component/RendererComponents/Mesh/MeshRenderer.h"


GameScene::GameScene() {}
GameScene::~GameScene() {}


void GameScene::Initialize() {
	Camera* camera = pEntityCollection_->GenerateCamera();
	camera->AddComponent<MeshRenderer>();

}

void GameScene::Update() {

}

