#include "Scene_Game.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "Input/Input.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/DemoObject/DemoObject.h"
#include "Objects/Player/Player.h"
#include "Objects/TrackingCamera/TrackingCamera.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	/// insatnce create
	std::vector<BaseGameObject*> createObjects;
	createObjects.reserve(16);
	createObjects.push_back(new Player());
	createObjects.push_back(new TrackingCamera(mainCamera_, createObjects[0]));


	/// initailizing
	for(BaseGameObject* obj : createObjects) {
		obj->Initialize();
	}

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {



}
