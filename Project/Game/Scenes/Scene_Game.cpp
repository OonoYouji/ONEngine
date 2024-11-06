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
#include "Objects/PlayerHPRenderer/PlayerHPRenderer.h"

/// lib
#include "Debugger/Assertion.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	Player* player = new Player();
	PlayerHPRenderer* playerHPRenderer = new PlayerHPRenderer();

	(new Stage)->Initialize();
	(new SkyDome)->Initialize();

	player->Initialize();
	playerHPRenderer->Initialize();

	playerHPRenderer->SetPlayer(player);

	/// ui layer  index=1
	GameCamera* uiCamera = new GameCamera("uiCamera");
	uiCamera->Initialize();
	uiCamera->SetDistance(10.0f);
	uiCamera->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
	AddLayer("ui", uiCamera);

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	

}
