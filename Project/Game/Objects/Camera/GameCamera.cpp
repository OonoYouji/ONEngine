#include "GameCamera.h"

#include"ImGuiManager/ImGuiManager.h"
#include"Math/Matrix4x4.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/Transform/Transform.h>

//function
#include"FrameManager/Time.h"

GameCamera::GameCamera(const std::string& name) {
	CreateTag(this);
	SetName(name);
	CameraManager::GetInstance()->AddCamera(GetName(), this);
}

GameCamera::~GameCamera() {}


/// ===================================================
/// 初期化
/// ===================================================
void GameCamera::Initialize() {
	BaseCamera::BaseInitialize();
}

/// ===================================================
/// 更新処理
/// ===================================================
void GameCamera::Update() {
	BaseUpdate();
}