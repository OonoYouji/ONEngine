#include "Vector3Int.h"

const Vector3Int Vector3Int::kOne = Vector3Int(1, 1, 1);
const Vector3Int Vector3Int::kZero = Vector3Int(0, 0, 0);

Vector3Int::Vector3Int() {
	*this = Vector3Int::kZero;
}

Vector3Int::Vector3Int(int32_t _x, int32_t _y, int32_t _z)
	: x(_x), y(_y), z(_z) {
}

Vector3Int::Vector3Int(const Vector3Int& _v)
	: x(_v.x), y(_v.y), z(_v.z) {
}

Vector3Int::Vector3Int(Vector3Int&& _v)
	: x(_v.x), y(_v.y), z(_v.z) {
}

Vector3Int::Vector3Int(const Vector3& _v)
	: x(static_cast<int32_t>(_v.x)),
	y(static_cast<int32_t>(_v.y)),
	z(static_cast<int32_t>(_v.z)) {
}

Vector3Int& Vector3Int::operator=(const Vector3Int& _other) {
	this->x = _other.x;
	this->y = _other.y;
	this->z = _other.z;
	return *this;
}

Vector3Int& Vector3Int::operator+=(const Vector3Int& _other) {
	(*this) = Vector3Int(this->x + _other.x, this->y + _other.y, this->z + _other.z);
	return *this;
}

Vector3Int& Vector3Int::operator-=(const Vector3Int& _other) {
	(*this) = Vector3Int(this->x - _other.x, this->y - _other.y, this->z - _other.z);
	return *this;
}

Vector3Int& Vector3Int::operator/=(const Vector3Int& _other) {
	(*this) = Vector3Int(this->x / _other.x, this->y / _other.y, this->z / _other.z);
	return *this;
}

Vector3Int& Vector3Int::operator*=(const Vector3Int& _other) {
	(*this) = Vector3Int(this->x * _other.x, this->y * _other.y, this->z * _other.z);
	return *this;
}

Vector3Int& Vector3Int::operator/=(int32_t _scaler) {
	(*this) = Vector3Int(this->x / _scaler, this->y / _scaler, this->z / _scaler);
	return *this;
}

Vector3Int& Vector3Int::operator*=(int32_t _scaler) {
	(*this) = Vector3Int(this->x * _scaler, this->y * _scaler, this->z * _scaler);
	return *this;
}

/// ===================================================
/// nlohmann::json 変換
/// ===================================================

void from_json(const nlohmann::json& _j, Vector3Int& _v) {
	_v.x = _j.at("x").get<int32_t>();
	_v.y = _j.at("y").get<int32_t>();
	_v.z = _j.at("z").get<int32_t>();
}

void to_json(nlohmann::json& _j, const Vector3Int& _v) {
	_j = {
		{ "x", _v.x },
		{ "y", _v.y },
		{ "z", _v.z },
	};
}
