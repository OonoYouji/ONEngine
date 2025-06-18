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

/// user: 全てのゲームオブジェクト
#include "Game/Effects/KeyItemEffects/KeyItemRipplesEffect.h"
#include "Game/Effects/KeyItemEffects/KeyItemRiseParticle.h"
#include "Game/Effects/PlaneEffects/PlayerMoveEffect.h"
#include "Game/Effects/PuzzleStandEffects/Cannon/PuzzleClearEffectCannon.h"
#include "Game/Effects/PuzzleStandEffects/Cannon/PuzzleClearEffectCannonStand.h"
#include "Game/Effects/PuzzleStandEffects/Effect/LaserEffect.h"
#include "Game/Effects/PuzzleStandEffects/Effect/LaserHitEffect.h"
#include "Game/Effects/PuzzleStandEffects/PuzzleClearEffect.h"

#include "Game/Entity/Player/Player.h"
#include "Game/Entity/TestEntity/TestEntity.h"

#include "Game/Objects/BackgroundObject.h"
#include "Game/Objects/Enemy.h"
#include "Game/Objects/KeyItem/KeyItem.h"
#include "Game/Objects/Puzzle/Puzzle.h"
#include "Game/Objects/Terrain/Terrain.h"

//#include "Game/Objects/Enemy.h"

GameScene::GameScene(EntityComponentSystem* _entityComponentSystem)
	: IScene(_entityComponentSystem) {

	/// このゲームで使用するエンティティをファクトリーに登録、一度GenerateEntity<T>()を呼び出すことでも登録される
	pEntityComponentSystem_->SetFactoryRegisterFunc(
		[&](EntityFactory* _factory) {
			_factory->Register("KeyItemRipplesEffect", []() { return std::make_unique<KeyItemRipplesEffect>(); });
			_factory->Register("KeyItemRiseParticle", []() { return std::make_unique<KeyItemRiseParticle>(); });
			_factory->Register("PlayerMoveEffect", []() { return std::make_unique<PlayerMoveEffect>(); });
			_factory->Register("PuzzleClearEffectCannon", []() { return std::make_unique<PuzzleClearEffectCannon>(); });
			_factory->Register("PuzzleClearEffectCannonStand", []() { return std::make_unique<PuzzleClearEffectCannonStand>(); });
			_factory->Register("LaserEffect", []() { return std::make_unique<LaserEffect>(); });
			_factory->Register("LaserHitEffect", []() { return std::make_unique<LaserHitEffect>(); });
			_factory->Register("PuzzleClearEffect", []() { return std::make_unique<PuzzleClearEffect>(); });

			_factory->Register("Player", []() { return std::make_unique<Player>(); });
			_factory->Register("TestEntity", []() { return std::make_unique<TestEntity>(); });

			_factory->Register("BackgroundObject", []() { return std::make_unique<BackgroundObject>(); });
			_factory->Register("Enemy", []() { return std::make_unique<Enemy>(); });
			_factory->Register("KeyItem", []() { return std::make_unique<KeyItem>(); });
			_factory->Register("Puzzle", []() { return std::make_unique<Puzzle>(); });
			_factory->Register("Terrain", []() { return std::make_unique<Terrain>(); });
		}
	);

}
GameScene::~GameScene() {}

void GameScene::Initialize() {

	pEntityComponentSystem_->GenerateEntity<Grid>();

	//terrainEditor_ = std::make_unique<TerrainEditor>(
	//	pEntityComponentSystem_->FindEntity<Terrain>(),
	//	pEntityComponentSystem_
	//);
	//terrainEditor_->Initialize();


	/// 草を生やす
	//EmptyEntity* emptyEntity = pEntityComponentSystem_->GenerateEntity<EmptyEntity>();

	//int num = 10;
	//for (size_t i = 0; i < num; i++) {
	//	for (size_t j = 0; j < num; j++) {
	//		auto grass = pEntityComponentSystem_->GenerateEntity<BackgroundObject>();
	//		grass->SetPosition(Vector3(i * 20, 0.0f, j * 20) + Random::Vector3(-Vec3::kOne * 10, Vec3::kOne * 10.0f));
	//		grass->SetPositionY(0.0f);
	//		grass->SetRotateY(Random::Float(0, 3));
	//		grass->SetScale(Vector3::kOne * Random::Float(1, 3));

	//		MeshRenderer* meshRenderer = grass->GetComponent<MeshRenderer>();
	//		if (meshRenderer) {
	//			int randomValue = rand() % 3 + 1;
	//			meshRenderer->SetMeshPath("./Packages/Models/BackgroundObjects/Tree" + std::to_string(randomValue) + ".obj");
	//		}

	//		grass->SetParent(emptyEntity);
	//	}
	//}


}

void GameScene::Update() {
#ifdef _DEBUG
	//terrainEditor_->Update();
#endif // _DEBUG

}

