#include "GameScene.h"

#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/Component/RendererComponents/Primitive/Line2DRenderer.h"

#include "Engine/Entity/Demo/DemoEntity.h"


GameScene::GameScene() {
	loadResourcePaths_ = {
		"Assets/Models/primitive/cube.obj",
		"Assets/Models/primitive/sphere.obj",
		"Assets/Models/primitive/cube.obj",
	};

	unloadResourcePaths_ = {
		"Assets/Models/cube/cube.obj"
	};
}

GameScene::~GameScene() {}


void GameScene::Initialize() {
	Camera* camera = pEntityCollection_->GenerateCamera();
	DemoEntity* entity = pEntityCollection_->GenerateEntity<DemoEntity>();

	Line2DRenderer* renderer = camera->AddComponent<Line2DRenderer>();
	renderer->SetLine(Vec2(0.0f, 360.0f), Vec2(1280.0f, 360.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	renderer->SetLine(Vec2(640.0f, 0.0f), Vec2(640.0f, 720.0f), Vec4(0.0f, 1.0f, 0.0f, 1.0f));

	entity;
}

void GameScene::Update() {

}

