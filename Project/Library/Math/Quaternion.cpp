#include "Quaternion.h"

#include <DirectXMath.h>


using namespace DirectX;


const Quaternion Quaternion::kIdentity = { 0.0f, 0.0f, 0.0f, 1.0f };



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
	/// startとendの内積を計算
	float dot = start.w * end.w + start.x * end.x + start.y * end.y + start.z * end.z;

	/// 内積が負の場合、endを反転してショートパスを取る
	Quaternion q0 = start;
	if(dot < 0.0f) {
		q0  = -q0;
		dot = -dot;
	}

	/// もし内積がほぼ1なら、線形補間を使う
	if(dot >= 1.0f - FLT_EPSILON) {
		return {
			((1.0f - t) * q0.x) + (t * end.x),
			((1.0f - t) * q0.y) + (t * end.y),
			((1.0f - t) * q0.z) + (t * end.z),
			((1.0f - t) * q0.w) + (t * end.w)
		};
	}

	/// なす角を計算
	float theta0 = std::acos(dot);
	float theta  = theta0 * t;

	/// sinを計算
	float sinTheta  = std::sin(theta);
	float sinTheta0 = std::sin(theta0);

	float s1 = std::cos(theta) - dot * sinTheta / sinTheta0;
	float s2 = sinTheta / sinTheta0;

	/// 補間したクォータニオンを計算
	return {
		(s1 * q0.x) + (s2 * end.x),
		(s1 * q0.y) + (s2 * end.y),
		(s1 * q0.z) + (s2 * end.z),
		(s1 * q0.w) + (s2 * end.w)
	};
}



Quaternion Quaternion::Inverse() const {
	Quaternion conjugate = this->Conjugate(); /// 共役を計算
	float len = this->Lenght();               /// 長さを計算
	if(len == 0.0f) {
		/// ノルムがゼロの場合、逆クォータニオンは定義されないため、適切なエラー処理を追加
		return *this; 
	}

	float lenSquared = len * len;    // 長さの二乗
	return conjugate / lenSquared;
}