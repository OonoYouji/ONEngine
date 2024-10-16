#include"Horming.h"
//std
#include<numbers>


Vec3 QuaternionToEulerAngles(const Quaternion& q) {
	Vec3 euler;

	// X軸（Roll）の計算
	float sinX = 2.0f * (q.y * q.z - q.x * q.w);
	float absSinX = std::abs(sinX);
	const float epsilon = 0.001f;

	// 特定の範囲内でsinXをゼロにする
	if (absSinX < epsilon) {
		sinX = 0.0f;
	}

	euler.x = std::asin(-sinX);  // Roll角度（X軸回転）

	// X軸の回転が90度付近の場合はジンバルロック状態になっている
	if (std::isnan(euler.x) || std::abs(std::abs(euler.x) - std::numbers::pi_v<float> / 2.0f) < epsilon) {
		euler.x = std::copysign(std::numbers::pi_v<float> / 2.0f, -sinX);
		return DealtWithZimbalLock(euler.x, q);
	}

	// X軸のcosを計算
	float cosX = std::cos(euler.x);

	// Y軸（Pitch）の計算
	float sinY = (2.0f * (q.x * q.z + q.y * q.w)) / cosX;
	float cosY = (2.0f * (q.w * q.w + q.z * q.z) - 1.0f) / cosX;
	euler.y = std::atan2(sinY, cosY);  // Pitch角度（Y軸回転）

	// Z軸（Yaw）の計算
	float sinZ = (2.0f * (q.x * q.y + q.z * q.w)) / cosX;
	float cosZ = (2.0f * (q.w * q.w + q.y * q.y) - 1.0f) / cosX;
	euler.z = std::atan2(sinZ, cosZ);  // Yaw角度（Z軸回転）

	return euler;
}

Vec3 DealtWithZimbalLock(float x, const Quaternion& q) {
	// X軸が90度近くになっているため、ジンバルロックが発生している
	// Y軸とZ軸の回転を調整する
	float y = std::atan2(2.0f * (q.x * q.z + q.y * q.w), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));

	// Z軸回転はジンバルロック時に曖昧になるため、Y軸だけで方向を決める
	// X軸が±90度のとき、Z軸の影響を無視するような回転
	float z = 0.0f;

	return { x, y, z };
}

//Vector3からQuaternion変換
Quaternion ToQuaternion(const Vec3& eulerAngles) {

	float halfRoll = eulerAngles.z * 0.5f;   // Roll
	float halfPitch = eulerAngles.x * 0.5f;  // Pitch
	float halfYaw = eulerAngles.y * 0.5f;    // Yaw

	float cosRoll = std::cos(halfRoll);
	float sinRoll = std::sin(halfRoll);
	float cosPitch = std::cos(halfPitch);
	float sinPitch = std::sin(halfPitch);
	float cosYaw = std::cos(halfYaw);
	float sinYaw = std::sin(halfYaw);

	Quaternion q;
	q.x = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw; // x
	q.y = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw; // y
	q.z = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw; // z
	q.w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw; // w

	return Quaternion::Normalize(q);
}

std::pair<float, float> CalculateDistanceAndDirection(const Vec3& targetPos, const Vec3& bossPosition, const float& radius) {
	// ボスとプレイヤーの位置を3次元座標から球面座標に変換
	float latitude1 = std::asin(bossPosition.y / Vector3::Length(bossPosition)); // ボスの緯度
	float longitude1 = std::atan2(bossPosition.z, bossPosition.x); // ボスの経度

	float latitude2 = std::asin(targetPos.y / Vector3::Length(targetPos)); // プレイヤーの緯度
	float longitude2 = std::atan2(targetPos.z, targetPos.x); // プレイヤーの経度

	// 中心角を計算
	float deltaSigma = std::acos(
		std::sin(latitude1) * std::sin(latitude2) +
		std::cos(latitude1) * std::cos(latitude2) * std::cos(longitude2 - longitude1)
	);

	// 球面上の距離を計算
	float sphereRadius = radius; // 半径
	float distance = sphereRadius * deltaSigma;

	// 方位角を計算
	float deltaLon = longitude2 - longitude1;
	float y = std::sin(deltaLon) * std::cos(latitude2);
	float x = std::cos(latitude1) * std::sin(latitude2) - std::sin(latitude1) * std::cos(latitude2) * std::cos(deltaLon);
	float direction = std::atan2(y, x);

	return { distance, direction }; // 距離と方位角を返す

}
