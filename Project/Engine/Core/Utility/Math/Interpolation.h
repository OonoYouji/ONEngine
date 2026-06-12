#pragma once

/// engine
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace ONEngine::Math {

/// @brief 線形補間
template <typename T>
inline T Lerp(const T& _a, const T& _b, float _t) {
    return _a + (_b - _a) * _t;
}

/// @brief ステップ（補間なし）
template <typename T>
inline T Step(const T& _a, const T& /*_b*/, float /*_t*/) {
    return _a;
}

/// @brief Vector2 の線形補間
inline Vector2 Lerp(const Vector2& _a, const Vector2& _b, float _t) {
    return { Lerp(_a.x, _b.x, _t), Lerp(_a.y, _b.y, _t) };
}

/// @brief Vector3 の線形補間
inline Vector3 Lerp(const Vector3& _a, const Vector3& _b, float _t) {
    return { Lerp(_a.x, _b.x, _t), Lerp(_a.y, _b.y, _t), Lerp(_a.z, _b.z, _t) };
}

/// @brief Vector4 の線形補間
inline Vector4 Lerp(const Vector4& _a, const Vector4& _b, float _t) {
    return { Lerp(_a.x, _b.x, _t), Lerp(_a.y, _b.y, _t), Lerp(_a.z, _b.z, _t), Lerp(_a.w, _b.w, _t) };
}

} /// namespace ONEngine::Math
