#include "Quaternion.h"

#include <DirectXMath.h>

/// std
#include <numbers>
#include <cmath>


using namespace DirectX;

namespace Engine::Math {

const Quaternion Quaternion::kIdentity = Quaternion(0.0f, 0.0f, 0.0f, 1.0f); ///< 単位クォータニオン


Quaternion::Quaternion() {
	*this = kIdentity;
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
	if(len != 0.0f) {
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

Quaternion Quaternion::MakeFromAxis(const Vector3& _axis, float _theta) {
	float halfAngle = _theta * 0.5f;
	float sinHalfAngle = std::sin(halfAngle);

	Vector3 normalizedAxis = _axis.Normalize();

	float w = std::cos(halfAngle);
	float x = normalizedAxis.x * sinHalfAngle;
	float y = normalizedAxis.y * sinHalfAngle;
	float z = normalizedAxis.z * sinHalfAngle;

	return Quaternion(x, y, z, w);
}

Matrix4x4 Quaternion::MakeRotateAxisAngle(const Vector3& _axis, float _theta) {
	return Matrix4x4::MakeRotate(MakeFromAxis(_axis, _theta));
}

Quaternion Quaternion::LookAt(const Vector3& _position, const Vector3& _target, const Vector3& _up) {
	/// ----- 視線の方向への回転を計算する ----- ///

	XMFLOAT3 xmPosition = { _position.x, _position.y, _position.z };
	XMFLOAT3 xmTarget = { _target.x, _target.y, _target.z };
	XMFLOAT3 xmUp = { _up.x, _up.y, _up.z };

	XMVECTOR posVec = XMLoadFloat3(&xmPosition);
	XMVECTOR targetVec = XMLoadFloat3(&xmTarget);
	XMVECTOR upVec = XMLoadFloat3(&xmUp);

	// 視線方向（forward）を計算
	XMVECTOR lookAtVec = XMVectorSubtract(targetVec, posVec);
	lookAtVec = XMVector3Normalize(lookAtVec);

	// forward と up がほぼ平行な場合は up を差し替える（真上 or 真下対策）
	float dot = XMVectorGetX(XMVector3Dot(lookAtVec, upVec));
	if(fabsf(dot) > 0.999f) {
		// Y軸とほぼ同一方向なのでZ軸をUpに使用
		upVec = XMVectorSet(0, 0, 1, 0);
	}

	// View 行列を作成（左手系）
	XMMATRIX viewMatrix = XMMatrixLookToLH(posVec, lookAtVec, upVec);

	// View は World の逆行列なので逆行列を取る（これがカメラのワールド行列になる）
	XMMATRIX worldMatrix = XMMatrixInverse(nullptr, viewMatrix);

	// worldMatrix から回転成分を取り出す
	XMVECTOR scalePart;
	XMVECTOR rotQuat;
	XMVECTOR transPart;
	bool decomposed = XMMatrixDecompose(&scalePart, &rotQuat, &transPart, worldMatrix);

	XMVECTOR finalQuat;
	if(decomposed) {
		finalQuat = rotQuat;
	} else {
		finalQuat = XMQuaternionRotationMatrix(worldMatrix);
	}

	XMFLOAT4 result;
	XMStoreFloat4(&result, finalQuat);

	return { result.x, result.y, result.z, result.w };
}



Quaternion Quaternion::LookAt(const Vector3& _position, const Vector3& _target) {
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

Quaternion Quaternion::FromEuler(const Vector3& _euler) {
	float x = _euler.x * 0.5f;
	float y = _euler.y * 0.5f;
	float z = _euler.z * 0.5f;

	float sx = std::sin(x);
	float cx = std::cos(x);
	float sy = std::sin(y);
	float cy = std::cos(y);
	float sz = std::sin(z);
	float cz = std::cos(z);

	Quaternion q;
	// XYZ order in Matrix corresponds to Q = Qz * Qy * Qx
	q.w = cx * cy * cz + sx * sy * sz;
	q.x = sx * cy * cz - cx * sy * sz;
	q.y = cx * sy * cz + sx * cy * sz;
	q.z = cx * cy * sz - sx * sy * cz;

	return Normalize(q);
}

Vector3 Quaternion::ToEuler(const Quaternion& _q) {
	// Matrix-based extraction for XYZ order
	Matrix4x4 m = Matrix4x4::MakeRotate(_q);
	Vector3 euler;

	// y = asin(-m02)
	float siny = -m.m[0][2];
	
	// Clamp siny to [-1, 1] to prevent NaN
	if (siny < -1.0f) siny = -1.0f;
	if (siny > 1.0f) siny = 1.0f;
	
	if (siny >= 0.9999f) {
		// Gimbal Lock: Y = 90 degrees
		euler.y = std::numbers::pi_v<float> / 2.0f;
		euler.x = std::atan2(m.m[1][0], m.m[1][1]);
		euler.z = 0.0f;
	} else if (siny <= -0.9999f) {
		// Gimbal Lock: Y = -90 degrees
		euler.y = -std::numbers::pi_v<float> / 2.0f;
		euler.x = std::atan2(-m.m[1][0], m.m[1][1]);
		euler.z = 0.0f;
	} else {
		euler.y = std::asin(siny);
		// x = atan2(m12, m22)
		euler.x = std::atan2(m.m[1][2], m.m[2][2]);
		// z = atan2(m01, m00)
		euler.z = std::atan2(m.m[0][1], m.m[0][0]);
	}

	return euler;
}

Quaternion Quaternion::FromRotationMatrix(const Matrix4x4& _m) {
	Quaternion q;

	float trace = _m.m[0][0] + _m.m[1][1] + _m.m[2][2];
	if(trace > 0.0f) {
		float s = std::sqrt(trace + 1.0f) * 2.0f; // S=4*qw
		q.w = 0.25f * s;
		q.x = (_m.m[1][2] - _m.m[2][1]) / s;
		q.y = (_m.m[2][0] - _m.m[0][2]) / s;
		q.z = (_m.m[0][1] - _m.m[1][0]) / s;
	} else {
		if(_m.m[0][0] > _m.m[1][1] && _m.m[0][0] > _m.m[2][2]) {
			float s = std::sqrt(1.0f + _m.m[0][0] - _m.m[1][1] - _m.m[2][2]) * 2.0f; // S=4*qx
			q.w = (_m.m[1][2] - _m.m[2][1]) / s;
			q.x = 0.25f * s;
			q.y = (_m.m[0][1] + _m.m[1][0]) / s;
			q.z = (_m.m[0][2] + _m.m[2][0]) / s;
		} else if(_m.m[1][1] > _m.m[2][2]) {
			float s = std::sqrt(1.0f + _m.m[1][1] - _m.m[0][0] - _m.m[2][2]) * 2.0f; // S=4*qy
			q.w = (_m.m[2][0] - _m.m[0][2]) / s;
			q.x = (_m.m[0][1] + _m.m[1][0]) / s;
			q.y = 0.25f * s;
			q.z = (_m.m[1][2] + _m.m[2][1]) / s;
		} else {
			float s = std::sqrt(1.0f + _m.m[2][2] - _m.m[0][0] - _m.m[1][1]) * 2.0f; // S=4*qz
			q.w = (_m.m[0][1] - _m.m[1][0]) / s;
			q.x = (_m.m[0][2] + _m.m[2][0]) / s;
			q.y = (_m.m[1][2] + _m.m[2][1]) / s;
			q.z = 0.25f * s;
		}
	}

	q = Normalize(q); // 正規化

	return q;
}


Quaternion Quaternion::Conjugate() const {
	return { -x, -y, -z, w };
}

float Quaternion::Length() const {
	return std::sqrt(w * w + x * x + y * y + z * z);
}

Quaternion Quaternion::Inverse() const {
	Quaternion conjugate = this->Conjugate(); // 共役を計算
	float norm = this->Length();                // ノルムを計算
	if(norm == 0.0f) {
		// ノルムがゼロの場合、逆クォータニオンは定義されないため、適切なエラー処理を追加
	}

	float normSquared = norm * norm;    // ノルムの二乗
	return conjugate / normSquared;
}

} /// namespace Engine::Math