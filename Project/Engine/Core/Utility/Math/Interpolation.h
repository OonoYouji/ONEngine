#pragma once

/// engine
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace ONEngine::Math {

/**
 * @brief 汎用的な線形補間（Lerp）を行います。
 * @param _a 開始値 (t=0)
 * @param _b 終了値 (t=1)
 * @param _t 補間パラメータ
 * @return 補間された値
 */
template <typename T>
inline T Lerp(const T& _a, const T& _b, float _t) {
    return _a + (_b - _a) * _t;
}

/**
 * @brief ステップ補間（補間を行わず、常に開始値を返す）を行います。
 * @param _a 開始値
 * @param _b 終了値（未使用）
 * @param _t 補間パラメータ（未使用）
 * @return 開始値 _a
 */
template <typename T>
inline T Step(const T& _a, const T& /*_b*/, float /*_t*/) {
    return _a;
}

/**
 * @brief Vector2 の線形補間を行います。
 * @param _a 開始座標
 * @param _b 終了座標
 * @param _t 補間パラメータ
 * @return 補間された Vector2
 */
inline Vector2 Lerp(const Vector2& _a, const Vector2& _b, float _t) {
    return { Lerp(_a.x, _b.x, _t), Lerp(_a.y, _b.y, _t) };
}

/**
 * @brief Vector3 の線形補間を行います。
 * @param _a 開始座標
 * @param _b 終了座標
 * @param _t 補間パラメータ
 * @return 補間された Vector3
 */
inline Vector3 Lerp(const Vector3& _a, const Vector3& _b, float _t) {
    return { Lerp(_a.x, _b.x, _t), Lerp(_a.y, _b.y, _t), Lerp(_a.z, _b.z, _t) };
}

/**
 * @brief Vector4 の線形補間を行います。
 * @param _a 開始座標
 * @param _b 終了座標
 * @param _t 補間パラメータ
 * @return 補間された Vector4
 */
inline Vector4 Lerp(const Vector4& _a, const Vector4& _b, float _t) {
    return { Lerp(_a.x, _b.x, _t), Lerp(_a.y, _b.y, _t), Lerp(_a.z, _b.z, _t), Lerp(_a.w, _b.w, _t) };
}

} /// namespace ONEngine::Math
