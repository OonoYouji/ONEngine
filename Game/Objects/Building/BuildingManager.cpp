#include"BuildingManager.h"

void 	BuildingManager::Init() {

}

void BuildingManager::SpownBuilding(float theta, float phi) {

	std::unique_ptr<Building> building = std::make_unique<Building>();
	building->Initialize();
	// 球面座標から位置を計算
	float x = 12 * sin(theta) * cos(phi);
	float y = 12 * sin(theta) * sin(phi);
	float z = 12 * cos(theta);

	// 回転を適用
	Quaternion rotateX = MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f },phi );
	Quaternion rotateY = MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, theta);
	building->SetPivotQuaternion(rotateX * rotateY);
	
	buildings_.push_back(std::move(building));
}

void 	BuildingManager::Update() {
	for (const std::unique_ptr<BaseBuilding>& building : buildings_) {
		building->Update();
	}

	buildings_.remove_if([](const std::unique_ptr<BaseBuilding>& building) {
		if (building->GetIsDeath()) {
			return true;
		}
		return false;
		});
}
void 	BuildingManager::Draw() {

}

Quaternion BuildingManager::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	//    W A   ɕϊ 
	float halfAngle = angle * 0.5f;
	float sinHalfAngle = sin(halfAngle);

	//    K     ꂽ   x N g  
	Vector3 normalizedAxis = axis.Normalize();

	//  N H [ ^ j I   ̐     v Z
	float w = cos(halfAngle);
	float x = normalizedAxis.x * sinHalfAngle;
	float y = normalizedAxis.y * sinHalfAngle;
	float z = normalizedAxis.z * sinHalfAngle;

	// Vector4  Ƃ  ĕԂ 
	return Quaternion(x, y, z, w);
}

Quaternion BuildingManager::LockAt(const Vec3& position, const Vec3& target, const Vec3& up) const {
	XMFLOAT3 xmPosition, xmTarget, xmUp;
	xmPosition = { position.x, position.y, position.z };
	xmTarget = { target.x,   target.y,   target.z };
	xmUp = { up.x,       up.y,       up.z };

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
