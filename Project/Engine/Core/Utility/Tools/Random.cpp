#include "Random.h"
#include <algorithm>

using namespace ONEngine;

std::mt19937 Random::generator_(std::random_device{}());

/**
 * @brief 範囲 [min, max] 内のランダムな整数を生成します。
 * @param _min 最小値
 * @param _max 最大値
 * @return 生成された整数
 */
int Random::Int(int _min, int _max) {
	std::uniform_int_distribution<int> distribution(_min, _max);
	return distribution(generator_);
}

/**
 * @brief 符号あり整数の全範囲からランダムな整数を生成します。
 * @return 生成された整数
 */
int Random::Int() {
	return Int((std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)());
}

/**
 * @brief 範囲 [min, max] 内のランダムな64ビット無符号整数を生成します。
 * @param _min 最小値
 * @param _max 最大値
 * @return 生成されたuint64_t値
 */
uint64_t Random::UInt64(uint64_t _min, uint64_t _max) {
	std::uniform_int_distribution<uint64_t> distribution(_min, _max);
	return distribution(generator_);
}

/**
 * @brief 64ビット無符号整数の全範囲からランダムな数値を生成します。
 * @return 生成されたuint64_t値
 */
uint64_t Random::UInt64() {
	return UInt64((std::numeric_limits<uint64_t>::min)(), (std::numeric_limits<uint64_t>::max)());
}

/**
 * @brief 範囲 [min, max] 内のランダムな単精度浮動小数点数を生成します。
 * @param _min 最小値
 * @param _max 最大値
 * @return 生成されたfloat値
 */
float Random::Float(float _min, float _max) {
	if (_min > _max) {
		std::swap(_min, _max);
	}
	std::uniform_real_distribution<float> distribution(_min, _max);
	return distribution(generator_);
}

/**
 * @brief 範囲 [0.0, 1.0] 内のランダムな単精度浮動小数点数を生成します。
 * @return 生成されたfloat値
 */
float Random::Float() {
	return Float((std::numeric_limits<float>::min)(), (std::numeric_limits<float>::max)());
}

/**
 * @brief 指定した最小・最大ベクトルの各軸範囲内でランダムな Vector2 ベクトルを生成します。
 * @param _min 最小境界
 * @param _max 最大境界
 * @return 生成されたVector2
 */
Vector2 Random::Vec2(const Vector2& _min, const Vector2& _max) {
	return Vector2(
		Float(_min.x, _max.x),
		Float(_min.y, _max.y)
	);
}

/**
 * @brief 指定した最小・最大ベクトルの各軸範囲内でランダムな Vector3 ベクトルを生成します。
 * @param _min 最小境界
 * @param _max 最大境界
 * @return 生成されたVector3
 */
Vector3 Random::Vec3(const Vector3& _min, const Vector3& _max) {
	return Vector3(
		Float(_min.x, _max.x),
		Float(_min.y, _max.y),
		Float(_min.z, _max.z)
	);
}

/**
 * @brief 指定した最小・最大ベクトルの各軸範囲内でランダムな Vector4 ベクトルを生成します。
 * @param _min 最小境界
 * @param _max 最大境界
 * @return 生成されたVector4
 */
Vector4 Random::Vec4(const Vector4& _min, const Vector4& _max) {
	return Vector4(
		Float(_min.x, _max.x),
		Float(_min.y, _max.y),
		Float(_min.z, _max.z),
		Float(_min.w, _max.w)
	);
}

/**
 * @brief 半径 1.0 の単位球体の内部にあるランダムな座標を生成します。
 * @return 球内のランダムなVector3座標
 */
Vector3 Random::InsideUnitSphere() {
	while (true) {
		Vector3 p = Vector3(Float(-1, 1), Float(-1, 1), Float(-1, 1));
		if (p.LengthSquared() < 1.0f) return p;
	}
}
