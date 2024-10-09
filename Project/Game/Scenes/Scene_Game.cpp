#include "Scene_Game.h"

#include "Objects/DemoObject/DemoObject.h"
#include "Objects/ShootingCourse/ShootingCourse.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	/// instance create...
	DemoObject* demoObj = new DemoObject;
	ShootingCourse* shootingCourse = new ShootingCourse();

	/// instance initializing...
	demoObj->Initialize();
	shootingCourse->Initialize();
	
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	
}
