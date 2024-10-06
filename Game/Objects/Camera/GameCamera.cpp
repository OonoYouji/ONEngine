#include "GameCamera.h"

#include "ImGuiManager.h"
#include"Matrix4x4.h"
#include <Component/MeshRenderer/MeshRenderer.h>
#include <Component/Transform/Transform.h>


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
	pTransform_->quaternion = { 0,0,0,1 };
	pTransform_->rotateOrder = QUATERNION;
	BaseCamera::BaseInitialize();

}

/// ===================================================
/// 更新処理
/// ===================================================
void GameCamera::Update() {

	/*Move();*/
	
	Transform* parent = GetParent();
	if(parent) {
		pTransform_->quaternion = LockAt(GetPosition(), parent->position, Vec3::kUp);
	}

	BaseUpdate();
}
void GameCamera::Debug()
{
	if (ImGui::TreeNodeEx("debyg", ImGuiTreeNodeFlags_DefaultOpen)) {

		Vec3 pos = GetPosition();
		ImGui::DragFloat3("world pos", &pos.x);
		ImGui::Text("m[0][0]:%f, m[0][1]:%f, m[0][2]:%f ,m[0][3]:%f", pTransform_->matTransform.m[0][0], pTransform_->matTransform.m[0][1], pTransform_->matTransform.m[0][2], pTransform_->matTransform.m[0][3]);
		ImGui::Text("m[1][0]:%f, m[1][1]:%f, m[1][2]:%f ,m[1][3]:%f", pTransform_->matTransform.m[1][0], pTransform_->matTransform.m[1][1], pTransform_->matTransform.m[1][2], pTransform_->matTransform.m[1][3]);
		ImGui::Text("m[2][0]:%f, m[2][1]:%f, m[2][2]:%f ,m[2][3]:%f", pTransform_->matTransform.m[2][0], pTransform_->matTransform.m[2][1], pTransform_->matTransform.m[2][2], pTransform_->matTransform.m[2][3]);
		ImGui::Text("m[3][0]:%f, m[3][1]:%f, m[3][2]:%f ,m[3][3]:%f", pTransform_->matTransform.m[3][0], pTransform_->matTransform.m[3][1], pTransform_->matTransform.m[3][2], pTransform_->matTransform.m[3][3]);

		ImGui::TreePop();
	}
}


Vec3 GameCamera::LockAt(const Vec3& direction) const {
	return {
		std::asin(direction.y), // arcsin(y)
		std::atan2(direction.x, direction.z), // atan2(x, z)
		0.0f
	};
}

Quaternion GameCamera::LockAt(const Vec3& position, const Vec3& target, const Vec3& up) const {
	XMFLOAT3 xmPosition, xmTarget, xmUp;
	xmPosition = { position.x, position.y, position.z };
	xmTarget   = { target.x,   target.y,   target.z };
	xmUp       = { up.x,       up.y,       up.z };

	// カメラの現在位置とターゲット方向ベクトルを定義
	XMVECTOR posVec = XMLoadFloat3(&xmPosition);  // カメラの位置
	XMVECTOR targetVec = XMLoadFloat3(&xmTarget); // カメラが向くターゲット位置
	XMVECTOR upVec = XMLoadFloat3(&xmUp);         // 上方向ベクトル

	// 視線方向ベクトルを計算
	XMVECTOR lookAtVec = XMVectorSubtract(targetVec, posVec);
	lookAtVec = XMVector3Normalize(lookAtVec); // 正規化

	// ビュー行列を作成
	XMMATRIX viewMatrix = XMMatrixLookToLH(posVec, lookAtVec, upVec);

	// ビュー行列をクォータニオンに変換
	XMVECTOR quaternion = XMQuaternionRotationMatrix(viewMatrix);

	// クォータニオン（XMFLOAT4）を返す
	XMFLOAT4 result;
	XMStoreFloat4(&result, quaternion);

	return { result.x, result.y, result.z, result.w };
}
