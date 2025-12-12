#include "Mathf.h"

/// std
#include <filesystem>
#include <fstream>

/// engine
#include "Engine/Core/Utility/Utility.h"

/// namespaceの短縮
namespace fs = std::filesystem;

using namespace ONEngine;

float Mathf::Cot(float _t) {
	/// 逆タンジェント
	return 1.0f / std::tan(_t);
}


uint32_t Mathf::DivideAndRoundUp(uint32_t _numerator, uint32_t _denominator) {
	if (_denominator == 0) {
		return 0; // ゼロ除算防止
	}

	return (_numerator + _denominator - 1) / _denominator;
}


Vector3 Mathf::CatmullRomPosition(const Vector3& _p0, const Vector3& _p1, const Vector3& _p2, const Vector3& _p3, float _t) {
	float t2 = _t * _t;
	float t3 = t2 * _t;

	return 0.5f * (
		(2.0f * _p1) +
		(-_p0 + _p2) * _t +
		(2.0f * _p0 - 5.0f * _p1 + 4.0f * _p2 - _p3) * t2 +
		(-_p0 + 3.0f * _p1 - 3.0f * _p2 + _p3) * t3);
}

bool ONEngine::Mathf::Inside(const Vector2& _point, const Vector2& _min, const Vector2& _max) {
	/// 点が矩形の内側にあるか判定
	return (_point.x >= _min.x && _point.x <= _max.x
		&& _point.y >= _min.y && _point.y <= _max.y);
}

Vector4 ONEngine::Mathf::ConvertToVector4(const Vector3& _v3, float _w) {
	return Vector4(
		_v3.x,
		_v3.y,
		_v3.z,
		_w
	);
}
