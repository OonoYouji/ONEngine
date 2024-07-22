#include "GameCamera.h"



GameCamera::GameCamera() {
	name_ = "GameCamera";
	CameraManager::GetInstance()->AddCamera(name_, this);
}

GameCamera::~GameCamera() {}


/// ===================================================
/// 初期化
/// ===================================================
void GameCamera::Initialize() {

	transform_.position = { 0.0f, 3.8f, -6.49f };
	transform_.rotate.x = 0.26f;

	BaseCamera::BaseInitialize();

}


/// ===================================================
/// 更新処理
/// ===================================================
void GameCamera::Update() {



}
