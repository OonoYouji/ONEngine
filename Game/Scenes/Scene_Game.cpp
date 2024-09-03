#include "Scene_Game.h"

#include <ImGuiManager.h> 
#include <ModelManager.h>

#include "Player/Player.h"


Scene_Game::Scene_Game() {}
Scene_Game::~Scene_Game() {}


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	model_ = ModelManager::CreateCube();
	model_->Initialize();

	sprite_.reset(new Sprite());
	sprite_->Initialize("uvChecker", "uvChecker.png");
	
	transform_.Initialize();

	(new Player)->Initialize();
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {



	transform_.UpdateMatrix();
}


/// ===================================================
/// 描画処理
/// ===================================================
void Scene_Game::Draw() {

	model_->Draw(&transform_);

	sprite_->Draw();

}
