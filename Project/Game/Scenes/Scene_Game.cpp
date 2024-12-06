#include "Scene_Game.h"

/// std
#include <numbers>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "Input/Input.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/DemoObject/DemoObject.h"
#include "Objects/Terrain/Terrain.h"

/// lib
#include "Debugger/Assertion.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	(new Terrain)->Initialize();

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

	if(Input::TriggerKey(KeyCode::A)) {
		SetNextScene("TitleScene");
	}


}
