#include "GameCamera.h"

#include "ImGuiManager/ImGuiManager.h"


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

	//transform_.position = { 0.0f, 3.8f, -6.49f * 2 };
	//transform_.rotate.x = 0.26f;

	pTranform_->position = { -2.221f, 3.245f, -27.257f};
	pTranform_->rotate.x = 0.215f;

	BaseCamera::BaseInitialize();

	/*pTranform_->rotateOrder = QUATERNION;*/

}

/// ===================================================
/// 更新処理
/// ===================================================
void GameCamera::Update() {

	/*Move();*/

	BaseUpdate();
}
void GameCamera::Debug()
{
	if (ImGui::TreeNodeEx("debyg", ImGuiTreeNodeFlags_DefaultOpen)) {

		Vec3 pos = GetPosition();
		ImGui::DragFloat3("world pos", &pos.x);

		ImGui::TreePop();
	}


}
//void  GameCamera::CalculateMatrixToView() {
//    // カメラの位置
//    Vector3 cameraPosition = transform_.GetPosition();
//
//    // カメラの向き（通常、前方を示すベクトル）
//    Vector3 cameraForward = transform_.GetForward(); // z軸
//    Vector3 cameraUp = transform_.GetUp(); // y軸
//
//    // 視点を基にビュー行列を計算
//    Vector3 zAxis = (cameraForward).Normalize(); // 前方ベクトルの正規化
//    Vector3 xAxis;
//    xAxis.Cross(cameraUp, zAxis).Normalize();
//    Vector3 yAxis; // z軸とx軸の外積
//    yAxis.Cross(zAxis, xAxis);
//
//    // ビュー行列の構成
//    Matrix4x4 viewMatrix = {
//        xAxis.x, xAxis.y, xAxis.z, 0.0f,
//        yAxis.x, yAxis.y, yAxis.z, 0.0f,
//        zAxis.x, zAxis.y, zAxis.z, 0.0f,
//        -(xAxis, cameraPosition).Dot(xAxis, cameraPosition),
//        -(yAxis, cameraPosition).Dot(yAxis, cameraPosition),
//       -(zAxis, cameraPosition).Dot(zAxis, cameraPosition),
//        1.0f
//    };
//
//    // ビュー行列をカメラに適用
//     return viewMatrix;
//}