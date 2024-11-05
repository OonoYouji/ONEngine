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
#include "Objects/Player/Player.h"
#include "Objects/Stage/Stage.h"
#include "Objects/SkyDome/SkyDome.h"

/// lib
#include "Debugger/Assertion.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	(new Player)->Initialize();
	(new Stage)->Initialize();
	(new SkyDome)->Initialize();

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	

}
