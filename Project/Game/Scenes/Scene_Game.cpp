#include "Scene_Game.h"

/// std
#include <numbers>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "Input/Input.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/DemoObject/DemoObject.h"

#include "EntityConponentSystem/Component/PositionComponent/PositionComponent.h"
#include "EntityConponentSystem/Component/VelocityComponent/VelocityComponent.h"
#include "EntityConponentSystem/Component/ColliderComponents/SphereColliderComponent.h"

#include "EntityConponentSystem/ECSManager/ECSManager.h"
#include "EntityConponentSystem/System/MovementSystem/MovementSystem.h"
#include "EntityConponentSystem/System/CollisionCheckerSystem/SphereCollisionSystem.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	demoObjA_ = new DemoObject();
	demoObjA_->Initialize();
	
	demoObjB_ = new DemoObject();
	demoObjB_->Initialize();


	ECSManager* ecsManager = ECSManager::GetInstance();
	ecsManager->Initialize();

	ecsManager->AddSystem(new MovementSystem());
	ecsManager->AddSystem(new SphereCollisionSystem());


	/// entity の作成
	entityA_ = ecsManager->GenerateEntity();
	
	SphereColliderComponent* sphereColliderA = ecsManager->AddComponent<SphereColliderComponent>(entityA_);
	sphereColliderA->radius = 5.0f;

	PositionComponent* posA = ecsManager->AddComponent<PositionComponent>(entityA_);
	posA->position = {};

	ecsManager->AddComponent<VelocityComponent>(entityA_);



	/// entity B create
	entityB_ = ecsManager->GenerateEntity();

	SphereColliderComponent* sphereColliderB = ecsManager->AddComponent<SphereColliderComponent>(entityB_);
	sphereColliderB->radius = 5.0f;

	PositionComponent* posB = ecsManager->AddComponent<PositionComponent>(entityB_);
	posB->position = { 2.0f, 0.0f, 0.0f };

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

	ECSManager* ecsManager = ECSManager::GetInstance();

	ecsManager->Update();

	PositionComponent* posAComp = ecsManager->GetComponent<PositionComponent>(entityA_);
	PositionComponent* posBComp = ecsManager->GetComponent<PositionComponent>(entityB_);
	
	demoObjA_->SetPosition(posAComp->position);
	demoObjB_->SetPosition(posBComp->position);

}
