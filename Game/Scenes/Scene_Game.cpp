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
	
	transform_.Initialize();

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
#ifdef _DEBUG
	ImGui::Begin("model setting");
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.005f);

	static bool isRotateX = false;
	static bool isRotateY = false;
	static bool isRotateZ = false;
	ImGui::Checkbox("isRotateX", &isRotateX);
	ImGui::Checkbox("isRotateY", &isRotateY);
	ImGui::Checkbox("isRotateZ", &isRotateZ);

	if(isRotateX) { transform_.rotate.x += 1.0f / 60.0f; }
	if(isRotateY) { transform_.rotate.y += 1.0f / 60.0f; }
	if(isRotateZ) { transform_.rotate.z += 1.0f / 60.0f; }


	ImGui::End();
#endif // _DEBUG

	transform_.UpdateMatrix();
}


/// ===================================================
/// 描画処理
/// ===================================================
void Scene_Game::Draw() {

	model_->Draw(&transform_);

	sprite_->Draw();

}
