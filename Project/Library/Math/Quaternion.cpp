#include "Quaternion.h"

#include <DirectXMath.h>


using namespace DirectX;


Quaternion::Quaternion() {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
}

Quaternion::Quaternion(float _x, float _y, float _z, float _w) {
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

float Quaternion::Lenght(const Quaternion& q) {
	return std::sqrt(
		q.x * q.x +
		q.y * q.y +
		q.z * q.z +
		q.w * q.w
	);
}

Quaternion Quaternion::Normalize(const Quaternion& q) {
	float len = Lenght(q);
	if(len != 0.0f) {
		return q / len;
	}
	return q;
}

Vector3 Quaternion::Transform(const Vector3& v, const Quaternion& q) {
	// ベクトルをクォータニオンに変換 (w = 0)
	Quaternion qVec = { v.x, v.y, v.z, 0.0f };

	// 回転: q * v * q^(-1)
	Quaternion qConjugate = q.Conjugate();
	Quaternion result = q * qVec * qConjugate;

	// 結果をベクトルとして返す
	return { result.x, result.y, result.z };
}

Quaternion Quaternion::MakeFromAxis(const Vec3& axis, float theta) {
	float halfAngle = theta * 0.5f;
	float sinHalfAngle = std::sin(halfAngle);

	Vector3 normalizedAxis = axis.Normalize();

	float w = std::cos(halfAngle);
	float x = normalizedAxis.x * sinHalfAngle;
	float y = normalizedAxis.y * sinHalfAngle;
	float z = normalizedAxis.z * sinHalfAngle;

	return Quaternion(x, y, z, w);
}

Quaternion Quaternion::LockAt(const Vec3& position, const Vec3& target, const Vec3& up) {
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



Quaternion Quaternion::Inverse() const {
	Quaternion conjugate = this->Conjugate(); // 共役を計算
	float norm = this->Lenght();                // ノルムを計算
	if(norm == 0.0f) {
		// ノルムがゼロの場合、逆クォータニオンは定義されないため、適切なエラー処理を追加
	}

	float normSquared = norm * norm;    // ノルムの二乗
	return conjugate / normSquared;
}