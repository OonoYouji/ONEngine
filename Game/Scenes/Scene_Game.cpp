#include "Scene_Game.h"

#include <ImGuiManager.h> 
#include <ModelManager.h>

#include "Player/Player.h"
#include "Submarine/Submarine.h"
#include "Guide/Guide.h"

Scene_Game::Scene_Game() {}
Scene_Game::~Scene_Game() {

	delete sinWave_;
	line2d_->Finalize();

}


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	model_ = ModelManager::CreateCube();
	model_->Initialize();
	
	transform_.Initialize();

	line2d_ = LineDrawer2D::GetInstance();
	line2d_->Initialize();

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

	//model_->Draw(&transform_);

	//sprite_->Draw();

	line2d_->PreDraw();


	sinWave_->Draw();


	line2d_->PostDraw();
}
