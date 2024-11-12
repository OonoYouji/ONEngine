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
#include "Objects/Enemy/Enemy.h"
#include "Objects/Player/Player.h"
#include "Objects/Stage/Stage.h"
#include "Objects/SkyDome/SkyDome.h"
#include "Objects/PlayerHPRenderer/PlayerHPRenderer.h"
#include "Objects/TrackingCamera/TrackingCamera.h"

/// lib
#include "Debugger/Assertion.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {


	/// object creata

	/// プレイヤー
	Player*           player           = new Player();
	PlayerHPRenderer* playerHPRenderer = new PlayerHPRenderer();

	/// 敵
	Enemy* enemy = new Enemy(player);

	/// 背景オブジェクト
	Stage*   stage   = new Stage();
	SkyDome* skyDome = new SkyDome();

	TrackingCamera*   trackingCamera   = new TrackingCamera(mainCamera_, player, enemy);

	/// 初期化する
	player->Initialize();
	playerHPRenderer->Initialize();
	trackingCamera->Initialize();
	enemy->Initialize();
	stage->Initialize();
	skyDome->Initialize();


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
