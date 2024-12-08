#include "Scene_Game.h"

/// objects
#include "Objects/Camera/GameCamera.h"

#include "Objects/Player/Player.h"
#include "Objects/BackgroundObjectManager/BackgroundObjectManager.h"
#include "Objects/TopDownCamera/TopDownCamera.h"

#include "Objects/Effect/RotationCube/RotationCube.h"
#include "Objects/OperationUI/OperationUI.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	Player*                  player        = new Player();
	BackgroundObjectManager* bbObjManager  = new BackgroundObjectManager();
	TopDownCamera*           topDownCamera = new TopDownCamera(mainCamera_, player);
	RotationCube* rotationCube = new RotationCube();
	OperationUI* operationUI = new OperationUI();

	
	player->Initialize();
	bbObjManager->Initialize();
	topDownCamera->Initialize();
	rotationCube->Initialize();
	operationUI->Initialize();

	player->SetTopDownCamera(topDownCamera);


	mainCamera_->SetPosition({ 0.0f, 23.2f, -27.7f });
	mainCamera_->SetRotate({ 0.65f, 0.0f, 0.0f });


	GameCamera* uiCamera = new GameCamera("uiCamera");
	uiCamera->Initialize();
	uiCamera->SetProjectionType(ORTHOGRAPHIC);

	AddLayer("uiLayer", uiCamera);
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

	


}
