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

Quaternion Quaternion::Lerp(const Quaternion& start, const Quaternion& end, float t) {
	return Quaternion(
		std::lerp(start.x, end.x, t),
		std::lerp(start.y, end.y, t),
		std::lerp(start.z, end.z, t),
		std::lerp(start.w, end.w, t)
	);
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

Mat4 Quaternion::MakeRotateAxisAngle(const Vec3& axis, float theta) {
	return Mat4::MakeRotateQuaternion(MakeFromAxis(axis, theta));
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

Quaternion Quaternion::LockAt(const Vec3& position, const Vec3& target) {
	XMFLOAT3 xmPosition, xmTarget;
	xmPosition = { position.x, position.y, position.z };
	xmTarget   = { target.x,   target.y,   target.z };

	// ベクトルに変換
	XMVECTOR eyeVec = XMLoadFloat3(&xmPosition);
	XMVECTOR targetVec = XMLoadFloat3(&xmTarget);

	// 前方ベクトルを計算
	XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(targetVec, eyeVec));

	// オイラー角の計算
	float yaw = std::atan2(XMVectorGetX(forward), XMVectorGetZ(forward));
	float pitch = std::asin(-XMVectorGetY(forward));

	// ロール角は不要なのでゼロとする
	float roll = 0.0f;

	// オイラー角をクォータニオンに変換
	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);

	// XMFLOAT4に変換して返す
	XMFLOAT4 result;
	XMStoreFloat4(&result, quaternion);

	return { result.x, result.y, result.z, result.w };
}

Quaternion Quaternion::Slerp(const Quaternion& start, const Quaternion& end, float t) {
	// startとendの内積を計算
	float dot = start.w * end.w + start.x * end.x + start.y * end.y + start.z * end.z;

	// 内積が負の場合、endを反転してショートパスを取る
	Quaternion q2Copy = end;
	if(dot < 0.0f) {
		dot = -dot;
		q2Copy.w = -q2Copy.w;
		q2Copy.x = -q2Copy.x;
		q2Copy.y = -q2Copy.y;
		q2Copy.z = -q2Copy.z;
	}

	// もし内積がほぼ1なら、線形補間を使う
	const float THRESHOLD = 0.9995f;
	if(dot > THRESHOLD) {
		Quaternion result = {
			start.x + t * (q2Copy.x - start.x),
			start.y + t * (q2Copy.y - start.y),
			start.z + t * (q2Copy.z - start.z),
			start.w + t * (q2Copy.w - start.w)
		};
		return Normalize(result);
	}

	// θを計算
	float theta_0 = std::acos(dot); // θ_0 = cos^(-1)(dot)
	float theta = theta_0 * t;      // θ = θ_0 * t

	// sinを計算
	float sin_theta = std::sin(theta);
	float sin_theta_0 = std::sin(theta_0);

	float s1 = std::cos(theta) - dot * sin_theta / sin_theta_0;
	float s2 = sin_theta / sin_theta_0;

	// 補間したクォータニオンを計算
	Quaternion result = {
		(s1 * start.x) + (s2 * q2Copy.x),
		(s1 * start.y) + (s2 * q2Copy.y),
		(s1 * start.z) + (s2 * q2Copy.z),
		(s1 * start.w) + (s2 * q2Copy.w)
	};
	return result;
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