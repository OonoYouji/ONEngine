#include "GameScene.h"

/// std
#include <numbers>

/// engine
#include "Engine/ECS/Entity/Entities/Demo/DemoEntity.h"
#include "Engine/ECS/Entity/Entities/Grid/Grid.h"
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"
#include "Engine/ECS/Entity/Entities/Light/DirectionalLightObject.h"
#include "Engine/ECS/Entity/Entities/Skybox/Skybox.h"

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

#include "Game/Objects/BackgroundObject.h"
#include "Game/Objects/Enemy.h"
#include "Game/Objects/KeyItem/KeyItem.h"
#include "Game/Objects/Puzzle/Puzzle.h"
#include "Game/Objects/Terrain/Terrain.h"

//#include "Game/Objects/Enemy.h"

GameScene::GameScene() {}
GameScene::~GameScene() {}


void GameScene::Initialize() {

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
			
			_factory->Register("BackgroundObject", []() { return std::make_unique<BackgroundObject>(); });
			_factory->Register("Enemy", []() { return std::make_unique<Enemy>(); });
			_factory->Register("KeyItem", []() { return std::make_unique<KeyItem>(); });
			_factory->Register("Puzzle", []() { return std::make_unique<Puzzle>(); });
			_factory->Register("Terrain", []() { return std::make_unique<Terrain>(); });
		}
	);

//
//	/// このゲームで使用するエンティティをファクトリーに登録、一度GenerateEntity<T>()を呼び出すことでも登録される
//	pEntityComponentSystem_->SetFactoryRegisterFunc(
//		[&](EntityFactory* _factory) {
//			_factory->Register("BackgroundObject", []() { return std::make_unique<BackgroundObject>(); });
//		}
//	);
//
//
//#ifdef _DEBUG
//	//pEntityComponentSystem_->GenerateEntity<Grid>();
//#endif // _DEBUG
//
//	//pEntityComponentSystem_->GenerateEntity<Enemy>();
//	pEntityComponentSystem_->GenerateEntity<DirectionalLightObject>();
//
//	//pEntityComponentSystem_->GenerateEntity<GameController>();
//	Player* player = pEntityComponentSystem_->GenerateEntity<Player>();
//	Camera* camera = pEntityComponentSystem_->GenerateCamera();
//	camera->SetParent(player);
//	camera->SetPosition(Vector3(0, 1.8f, -2.5f));
//	camera->SetRotateX(std::numbers::pi_v<float> *0.1f);
//	player->SetCamera(camera);
//
//	Skybox* skybox = pEntityComponentSystem_->GenerateEntity<Skybox>();
//	skybox->SetScale(Vector3::kOne * 500.0f);
//
//	pEntityComponentSystem_->SetMainCamera(camera);
//	KeyItem* keyItem = pEntityComponentSystem_->GenerateEntity<KeyItem>();
//	keyItem->SetPosition(Vector3(-20, 0, 0));
//	keyItem->UpdateTransform();
//
	//terrainEditor_ = std::make_unique<TerrainEditor>(
	//	pEntityComponentSystem_->FindEntity<Terrain>(),
	//	pEntityComponentSystem_
	//);
	//terrainEditor_->Initialize();
//
//	pEntityComponentSystem_->GenerateEntity<PuzzleClearEffect>();
//
//
//	/// puzzle
//	pEntityComponentSystem_->GenerateEntity<Puzzle>();
}

void GameScene::Update() {
#ifdef _DEBUG
#endif // _DEBUG

}

