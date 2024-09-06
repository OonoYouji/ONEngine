#include "Scene_Game.h"

#include <ImGuiManager.h> 
#include <ModelManager.h>

#include "Player/Player.h"


Scene_Game::Scene_Game() {}
Scene_Game::~Scene_Game() {

	delete sinWave_;

}


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	model_ = ModelManager::CreateCube();
	model_->Initialize();

	sprite_.reset(new Sprite());
	sprite_->Initialize("uvChecker", "uvChecker.png");
	sprite_->SetColor({ 1,1,1,0.5f });

	transform_.Initialize();

	(new Player)->Initialize();

	sinWave_ = new SinWaveDrawer();
	sinWave_->Initialize();

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	
	sinWave_->Update();

	transform_.UpdateMatrix();
}


/// ===================================================
/// 描画処理
/// ===================================================
void Scene_Game::Draw() {

	model_->Draw(&transform_);
	sprite_->Draw();

	//sinWave_->Draw();

}
