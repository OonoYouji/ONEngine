#include "Scene_Game.h"

/// objects
#include "Objects/Camera/GameCamera.h"

#include "Objects/Player/Player.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	(new Player())->Initialize();


	mainCamera_->SetPosition({ 0, 5.5f, -17.0f });
	mainCamera_->SetRotate({ 0.25f, 0.0f, 0.0f });
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

	


}
