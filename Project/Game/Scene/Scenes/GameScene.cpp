#include "GameScene.h"

/// std
#include <numbers>

/// engine
#include "Engine/ECS/Entity/Entities/Demo/DemoEntity.h"
#include "Engine/ECS/Entity/Entities/EmptyEntity/EmptyEntity.h"
#include "Engine/ECS/Entity/Entities/Grid/Grid.h"
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"
#include "Engine/ECS/Entity/Entities/Light/DirectionalLightObject.h"
#include "Engine/ECS/Entity/Entities/Skybox/Skybox.h"
#include "Engine/ECS/Component/Component.h"

GameScene::GameScene(EntityComponentSystem* _entityComponentSystem, const std::string& _name)
	: IScene(_entityComponentSystem, _name) {

	// このゲームで使用するエンティティをファクトリーに登録、一度GenerateEntity<T>()を呼び出すことでも登録される
	pEntityComponentSystem_->SetFactoryRegisterFunc(
		[&](EntityFactory* _factory) {
			_factory->Register("Terrain", []() { return std::make_unique<Terrain>(); });
		}
	);

}
GameScene::~GameScene() {}

void GameScene::Initialize() {

	Terrain* terrain = pEntityComponentSystem_->FindEntity<Terrain>();
	terrainEditor_ = std::make_unique<TerrainEditor>(
		terrain, pEntityComponentSystem_
	);
	terrainEditor_->Initialize();
	//terrain->InputVertices();

}

void GameScene::Update() {
#ifdef _DEBUG
	terrainEditor_->Update();
#endif // _DEBUG

}

