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



void GameCamera::Debug() {

	

	if(ImGui::TreeNodeEx("debyg", ImGuiTreeNodeFlags_DefaultOpen)) {

		Vec3 pos = GetPosition();
		ImGui::DragFloat3("world pos", &pos.x);
		ImGui::Text("m[0][0]:%f, m[0][1]:%f, m[0][2]:%f ,m[0][3]:%f", pTransform_->matTransform.m[0][0], pTransform_->matTransform.m[0][1], pTransform_->matTransform.m[0][2], pTransform_->matTransform.m[0][3]);
		ImGui::Text("m[1][0]:%f, m[1][1]:%f, m[1][2]:%f ,m[1][3]:%f", pTransform_->matTransform.m[1][0], pTransform_->matTransform.m[1][1], pTransform_->matTransform.m[1][2], pTransform_->matTransform.m[1][3]);
		ImGui::Text("m[2][0]:%f, m[2][1]:%f, m[2][2]:%f ,m[2][3]:%f", pTransform_->matTransform.m[2][0], pTransform_->matTransform.m[2][1], pTransform_->matTransform.m[2][2], pTransform_->matTransform.m[2][3]);
		ImGui::Text("m[3][0]:%f, m[3][1]:%f, m[3][2]:%f ,m[3][3]:%f", pTransform_->matTransform.m[3][0], pTransform_->matTransform.m[3][1], pTransform_->matTransform.m[3][2], pTransform_->matTransform.m[3][3]);

		ImGui::TreePop();
	}
}
