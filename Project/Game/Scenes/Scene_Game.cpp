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

#include "EntityConponentSystem/Component/ComponentManager/ComponentManager.h"
#include "EntityConponentSystem/Component/PositionComponent.h"
#include "EntityConponentSystem/Component/VelocityComponent.h"

/// lib
#include "Debugger/Assertion.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	demoObj_ = new DemoObject();
	demoObj_->Initialize();

	ComponentManager::GetInstance()->Initialize();

	moveSystem_.reset(new MoveSystem);
	moveSystem_->Initialize();


	VelocityComponent* velocity = demoEntity_.GetComponent<VelocityComponent>();
	velocity->velocity_ = Vec3::kFront;

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	ComponentManager::GetInstance()->Update();

	PositionComponent* position = demoEntity_.GetComponent<PositionComponent>();
	demoObj_->SetPosition(position->position_);

	moveSystem_->Update(&demoEntity_);
}
