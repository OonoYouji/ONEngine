#include "GameCamera.h"

#include"ImGuiManager/ImGuiManager.h"
#include"Math/Matrix4x4.h"
#include <ComponentManager/MeshRenderer/MeshRenderer.h>
#include <ComponentManager/Transform/Transform.h>


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
	//auto meshRenderer = AddComponent<MeshRenderer>();
	//meshRenderer->SetModel("TestObject");
	//transform_.position = { 0.0f, 3.8f, -6.49f * 2 };
	//transform_.rotate.x = 0.26f;

	pTransform_->quaternion  = { 0, 0, 0, 1 };
	pTransform_->rotateOrder = QUATERNION;
	BaseCamera::BaseInitialize();

}

/// ===================================================
/// 更新処理
/// ===================================================
void GameCamera::Update() {
	
	//更新
	BaseUpdate();
}


