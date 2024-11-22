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

/// lib
#include "Debugger/Assertion.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	Model* sphere = ModelManager::Load("Sphere");


	DemoObject* demoObjA = new DemoObject();
	demoObjA->Initialize();
	demoObjA->AddComponent<SphereCollider>(sphere);
	
	DemoObject* demoObjB = new DemoObject();
	demoObjB->Initialize();
	demoObjB->AddComponent<BoxCollider>(sphere);

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {


}
