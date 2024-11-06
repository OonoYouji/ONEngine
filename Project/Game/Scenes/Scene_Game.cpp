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
#include "Objects/DemoBoxColliderOBJ/DemoBoxColliderOBJ.h"
#include "Objects/DemoSphereColliderOBJ/DemoSphereColliderOBJ.h"

/// lib
#include "Debugger/Assertion.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	(new DemoObject)->Initialize();
	//(new DemoBoxColliderOBJ)->Initialize();
	(new DemoSphereColliderOBJ)->Initialize();

	mainCamera_->SetPosition({ 0.0f, 0.0f, -5.0f });
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	

}
