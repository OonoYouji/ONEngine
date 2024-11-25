#include "Scene_Game.h"

/// std
#include <numbers>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"

/// components
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include "ComponentManager/Collider/SphereCollider.h"
#include "ComponentManager/Collider/BoxCollider.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/DemoObject/DemoObject.h"

#include "EntityConponentSystem/Component/PositionComponent/PositionComponent.h"
#include "EntityConponentSystem/Component/VelocityComponent/VelocityComponent.h"
#include "EntityConponentSystem/Component/ColliderComponents/SphereColliderComponent.h"

#include "EntityConponentSystem/ECSManager/ECSManager.h"
#include "EntityConponentSystem/System/MovementSystem/MovementSystem.h"
#include "EntityConponentSystem/System/CollisionCheckerSystem/SphereCollisionSystem.h"

/// lib
#include "Debugger/Assertion.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	demoObj_ = new DemoObject();
	demoObj_->Initialize();

	ECSManager* ecsManager = ECSManager::GetInstance();
	ecsManager->Initialize();

	ecsManager->AddSystem(new MovementSystem());
	ecsManager->AddSystem(new SphereCollisionSystem());


	/// entity の作成
	entity_ = ecsManager->GenerateEntity();
	
	ecsManager->AddComponent<SphereColliderComponent>(entity_);

	ecsManager->AddComponent<PositionComponent>(entity_);
	VelocityComponent* veloComp = ecsManager->AddComponent<VelocityComponent>(entity_);
	veloComp->velocity = Vec3::kFront * 0.1f;
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

	ECSManager* ecsManager = ECSManager::GetInstance();

	ecsManager->Update();

	PositionComponent* posComp = ecsManager->GetComponent<PositionComponent>(entity_);
	
	demoObj_->SetPosition(posComp->position);

}
