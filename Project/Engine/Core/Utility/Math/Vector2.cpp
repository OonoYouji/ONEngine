#include "Vector2.h"

using namespace ONEngine;

#include <cmath>

/// ===================================================
/// static objects initialize
/// ===================================================

const Vector2 Vector2::kUp = Vector2(0, 1);		//- ( 0,  1 )
const Vector2 Vector2::kDown = Vector2(0, -1);	//- ( 0, -1 )
const Vector2 Vector2::kLeft = Vector2(-1, 0);	//- (-1,  0 )
const Vector2 Vector2::kRight = Vector2(1, 0);	//- ( 1,  0 )
const Vector2 Vector2::kOne = Vector2(1, 1);	//- ( 1,  1 )
const Vector2 Vector2::kZero = Vector2(0, 0);	//- ( 0,  0 )

const Vector2 Vector2::kHD = Vector2(1280.0f, 720.0f);
const Vector2 Vector2::kFullHD = Vector2(1920.0f, 1080.0f);


Vector2::Vector2() {
	this->x = 0.0f;
	this->y = 0.0f;
}

Vector2::Vector2(float _x, float _y) {
	this->x = _x;
	this->y = _y;
}

float Vector2::Length() const {
	return std::sqrt(x * x + y * y);
}

Vector2 Vector2::Normalize() const {
	float len = this->Length();
	if (len != 0.0f) {
		return (*this) / len;
	}
	return *this;
}

float Vector2::Dot(const Vector2& _v1, const Vector2& _v2) {
	return (_v1.x * _v2.x) + (_v1.y * _v2.y);
}

Vector2 Vector2::Lerp(const Vector2& _v1, const Vector2& _v2, float _t) {
	return Vector2(
		std::lerp(_v1.x, _v2.x, _t),
		std::lerp(_v1.y, _v2.y, _t)
	);
}

bool Vector2::Inside(const Vector2& _point, const Vector2& _min, const Vector2& _max) {
	/// 点が矩形の内側にあるか判定
	return (_point.x >= _min.x && _point.x <= _max.x
		&& _point.y >= _min.y && _point.y <= _max.y);
}

void ONEngine::from_json(const nlohmann::json& _j, Vector2& _v) {
	_v.x = _j.at("x").get<float>();
	_v.y = _j.at("y").get<float>();
}

void ONEngine::to_json(nlohmann::json& _j, const Vector2& _v) {
	_j = nlohmann::json{ { "x", _v.x }, { "y", _v.y } };
}
