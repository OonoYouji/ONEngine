#include "Scene_Game.h"

/// objects
#include "Objects/Camera/GameCamera.h"

#include "Objects/Player/Player.h"
#include "Objects/BackgroundObjectManager/BackgroundObjectManager.h"
#include "Objects/TopDownCamera/TopDownCamera.h"

#include "Objects/ParticleEditor/ParticleEditor.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	ParticleEditor* particleEditor = new ParticleEditor();

	Player*                  player        = new Player();
	BackgroundObjectManager* bbObjManager  = new BackgroundObjectManager();
	TopDownCamera*           topDownCamera = new TopDownCamera(mainCamera_, player);
	
	particleEditor->Initialize();
	player->Initialize();
	bbObjManager->Initialize();
	topDownCamera->Initialize();

	player->SetTopDownCamera(topDownCamera);


	mainCamera_->SetPosition({ 0.0f, 23.2f, -27.7f });
	mainCamera_->SetRotate({ 0.65f, 0.0f, 0.0f });
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

	


}
