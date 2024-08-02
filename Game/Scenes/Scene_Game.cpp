#include "Scene_Game.h"

#include <ModelManager.h>

#include <ImGuiManager.h>


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
	
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
#ifdef _DEBUG
	ImGui::Begin("model setting");
	ImGui::DragFloat3("rotate", &rotate_.x, 0.005f);

	static bool isRotateX = false;
	static bool isRotateY = false;
	static bool isRotateZ = false;
	ImGui::Checkbox("isRotateX", &isRotateX);
	ImGui::Checkbox("isRotateY", &isRotateY);
	ImGui::Checkbox("isRotateZ", &isRotateZ);

	if(isRotateX) { rotate_.x += 1.0f / 60.0f; }
	if(isRotateY) { rotate_.y += 1.0f / 60.0f; }
	if(isRotateZ) { rotate_.z += 1.0f / 60.0f; }


	ImGui::End();
#endif // _DEBUG


	model_->SetRotate(rotate_);

}


/// ===================================================
/// 描画処理
/// ===================================================
void Scene_Game::Draw() {

	model_->Draw();

	sprite_->Draw();

}
