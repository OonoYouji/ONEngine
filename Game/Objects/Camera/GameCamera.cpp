#include "GameCamera.h"



GameCamera::GameCamera() {
	SetName("GameCamera");
	CameraManager::GetInstance()->AddCamera(GetName(), this);
}

GameCamera::~GameCamera() {}


/// ===================================================
/// 初期化
/// ===================================================
void GameCamera::Initialize() {

	//transform_.position = { 0.0f, 3.8f, -6.49f * 2 };
	//transform_.rotate.x = 0.26f;

	transform_.position = { -2.221f, 3.245f, -27.257f};
	transform_.rotate.x = 0.215f;

	BaseCamera::BaseInitialize();

}


/// ===================================================
/// 更新処理
/// ===================================================
void GameCamera::Update() {

	UpdateMatrix();
	UpdateMatView();
	UpdateMatProjection();
	Transfer();

}
