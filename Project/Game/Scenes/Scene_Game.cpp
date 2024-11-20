#include "Scene_Game.h"

/// std
#include <numbers>

/// engine
#include <ImGuiManager/ImGuiManager.h>

/// components
#include <ComponentManager/MeshRenderer/MeshRenderer.h>

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/DemoObject/DemoObject.h"

#include "EntityConponentSystem/Component/PositionComponent/PositionComponent.h"
#include "EntityConponentSystem/Component/VelocityComponent/VelocityComponent.h"

#include "EntityConponentSystem/ECSManager/ECSManager.h"

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

	ecsManager->AddSystem(new MoveSystem());


	entity_ = ecsManager->GenerateEntity();
	ecsManager->AddComponent<PositionComponent>(entity_);
	VelocityComponent* veloComp = ecsManager->AddComponent<VelocityComponent>(entity_);
	veloComp->velocity_ = Vec3::kFront * 0.1f;
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

	ECSManager* ecsManager = ECSManager::GetInstance();

	ecsManager->Update();

	PositionComponent* posComp = ecsManager->GetComponent<PositionComponent>(entity_);
	
	demoObj_->SetPosition(posComp->position_);

}
