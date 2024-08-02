#include <DebugCamera.h>

#include <CameraManager.h>

DebugCamare::DebugCamare() {
	name_ = "DebugCamera";
	CameraManager::GetInstance()->AddCamera(name_, this);
}
DebugCamare::~DebugCamare() {}


/// ===================================================
/// 初期化
/// ===================================================
void DebugCamare::Initialize() {

	transform_.position = { 0.0f, 0.0f, -6.49f * 2 };

	BaseCamera::BaseInitialize();
}


/// ===================================================
/// 描画
/// ===================================================
void DebugCamare::Update() {

}
