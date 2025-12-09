#pragma once

/// std
#include <cstdint>

/// engine
#include "Vector3.h"


/// ///////////////////////////////////////////////////
/// Vector3の整数型
/// ///////////////////////////////////////////////////
struct Vector3Int {

	Vector3Int();
	Vector3Int(int32_t _x, int32_t _y, int32_t _z);
	Vector3Int(const Vector3Int& _v);
	Vector3Int(Vector3Int&& _v);
	Vector3Int(const Vector3& _v);

	int32_t x, y, z;

	static const Vector3Int kOne;   ///< (1, 1, 1)
	static const Vector3Int kZero;  ///< (0, 0, 0)

	Vector3Int& operator= (const Vector3Int& _other);
	Vector3Int& operator+= (const Vector3Int& _other);
	Vector3Int& operator-= (const Vector3Int& _other);
	Vector3Int& operator/= (const Vector3Int& _other);
	Vector3Int& operator*= (const Vector3Int& _other);
	Vector3Int& operator/= (int32_t _scaler);
	Vector3Int& operator*= (int32_t _scaler);
};


/// ===================================================
/// 演算子オーバーロード
/// ===================================================

inline Vector3Int operator+ (const Vector3Int& _v1, const Vector3Int& _v2) {
	return {
		_v1.x + _v2.x,
		_v1.y + _v2.y,
		_v1.z + _v2.z
	};
}

inline Vector3Int operator- (const Vector3Int& _v1, const Vector3Int& _v2) {
	return {
		_v1.x - _v2.x,
		_v1.y - _v2.y,
		_v1.z - _v2.z
	};
}

inline Vector3Int operator/ (const Vector3Int& _v1, const Vector3Int& _v2) {
	return {
		_v1.x / _v2.x,
		_v1.y / _v2.y,
		_v1.z / _v2.z
	};
}

inline Vector3Int operator/ (const Vector3Int& _v, int32_t _scaler) {
	return {
		_v.x / _scaler,
		_v.y / _scaler,
		_v.z / _scaler
	};
}

inline Vector3Int operator* (const Vector3Int& _v1, const Vector3Int& _v2) {
	return {
		_v1.x * _v2.x,
		_v1.y * _v2.y,
		_v1.z * _v2.z
	};
}

inline Vector3Int operator* (const Vector3Int& _v, int32_t _scaler) {
	return {
		_v.x * _scaler,
		_v.y * _scaler,
		_v.z * _scaler
	};
}

inline Vector3Int operator* (int32_t _scaler, const Vector3Int& _v) {
	return _v * _scaler;
}


/// ====================================================
/// Json 変換
/// ====================================================

void from_json(const nlohmann::json& _j, Vector3Int& _v);
void to_json(nlohmann::json& _j, const Vector3Int& _v);