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

float Quaternion::Length(const Quaternion& _q) {
	return std::sqrt(
		_q.x * _q.x +
		_q.y * _q.y +
		_q.z * _q.z +
		_q.w * _q.w
	);
}

Quaternion Quaternion::Normalize(const Quaternion& _q) {
	float len = Length(_q);
	if (len != 0.0f) {
		return _q / len;
	}
	return _q;
}

Vector3 Quaternion::Transform(const Vector3& _v, const Quaternion& _q) {
	// ベクトルをクォータニオンに変換 (w = 0)
	Quaternion qVec = { _v.x, _v.y, _v.z, 0.0f };

	// 回転: _q * _v * _q^(-1)
	Quaternion qConjugate = _q.Conjugate();
	Quaternion result = _q * qVec * qConjugate;

	// 結果をベクトルとして返す
	return { result.x, result.y, result.z };
}

Quaternion Quaternion::Lerp(const Quaternion& _start, const Quaternion& _end, float _t) {
	return Quaternion(
		std::lerp(_start.x, _end.x, _t),
		std::lerp(_start.y, _end.y, _t),
		std::lerp(_start.z, _end.z, _t),
		std::lerp(_start.w, _end.w, _t)
	);
}

Quaternion Quaternion::MakeFromAxis(const Vec3& _axis, float _theta) {
	float halfAngle = _theta * 0.5f;
	float sinHalfAngle = std::sin(halfAngle);

	Vector3 normalizedAxis = _axis.Normalize();

	float w = std::cos(halfAngle);
	float x = normalizedAxis.x * sinHalfAngle;
	float y = normalizedAxis.y * sinHalfAngle;
	float z = normalizedAxis.z * sinHalfAngle;

	return Quaternion(x, y, z, w);
}

Matrix4x4 Quaternion::MakeRotateAxisAngle(const Vec3& _axis, float _theta) {
	return Matrix4x4::MakeRotateQuaternion(MakeFromAxis(_axis, _theta));
}

Quaternion Quaternion::LockAt(const Vec3& _position, const Vec3& _target, const Vec3& _up) {
	XMFLOAT3 xmPosition, xmTarget, xmUp;
	xmPosition = { _position.x, _position.y, _position.z };
	xmTarget = { _target.x, _target.y, _target.z };
	xmUp = { _up.x, _up.y, _up.z };

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

Quaternion Quaternion::LockAt(const Vec3& _position, const Vec3& _target) {
	XMFLOAT3 xmPosition, xmTarget;
	xmPosition = { _position.x, _position.y, _position.z };
	xmTarget = { _target.x, _target.y, _target.z };

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

Quaternion Quaternion::Slerp(const Quaternion& _start, const Quaternion& _end, float _t) {
	// _startと_endの内積を計算
	float dot = _start.w * _end.w + _start.x * _end.x + _start.y * _end.y + _start.z * _end.z;

	// 内積が負の場合、_endを反転してショートパスを取る
	Quaternion q2Copy = _end;
	if (dot < 0.0f) {
		dot = -dot;
		q2Copy.w = -q2Copy.w;
		q2Copy.x = -q2Copy.x;
		q2Copy.y = -q2Copy.y;
		q2Copy.z = -q2Copy.z;
	}

	// もし内積がほぼ1なら、線形補間を使う
	const float THRESHOLD = 0.9995f;
	if (dot > THRESHOLD) {
		Quaternion result = {
			_start.x + _t * (q2Copy.x - _start.x),
			_start.y + _t * (q2Copy.y - _start.y),
			_start.z + _t * (q2Copy.z - _start.z),
			_start.w + _t * (q2Copy.w - _start.w)
		};
		return Normalize(result);
	}

	// θを計算
	float theta_0 = std::acos(dot); // θ_0 = cos^(-1)(dot)
	float theta = theta_0 * _t;      // θ = θ_0 * _t

	// sinを計算
	float sin_theta = std::sin(theta);
	float sin_theta_0 = std::sin(theta_0);

	float s1 = std::cos(theta) - dot * sin_theta / sin_theta_0;
	float s2 = sin_theta / sin_theta_0;

	// 補間したクォータニオンを計算
	Quaternion result = {
		(s1 * _start.x) + (s2 * q2Copy.x),
		(s1 * _start.y) + (s2 * q2Copy.y),
		(s1 * _start.z) + (s2 * q2Copy.z),
		(s1 * _start.w) + (s2 * q2Copy.w)
	};
	return result;
}



Quaternion Quaternion::Inverse() const {
	Quaternion conjugate = this->Conjugate(); // 共役を計算
	float norm = this->Length();                // ノルムを計算
	if (norm == 0.0f) {
		// ノルムがゼロの場合、逆クォータニオンは定義されないため、適切なエラー処理を追加
	}

	float normSquared = norm * norm;    // ノルムの二乗
	return conjugate / normSquared;
}