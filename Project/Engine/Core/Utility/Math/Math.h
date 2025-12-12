#pragma once

/// std
#include <numbers>
#include <string>
#include <vector>

/// engine
#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"

namespace ONEngine {

/// //////////////////////////////////////////////////
/// よく使う関数やちょっと便利な関数をまとめた名前空間
/// //////////////////////////////////////////////////
namespace Math {

static const float Deg2Rad = std::numbers::pi_v<float> / 180.0f;
static const float Rad2Deg = 180.0f / std::numbers::pi_v<float>;
static const float PI      = std::numbers::pi_v<float>;
static const float PI2     = std::numbers::pi_v<float> * 2.0f;


/// @brief 逆タンジェント
/// @param _t 
/// @return 
float Cot(float _t);


/// @brief 分子を分母で割り、余りがある場合は切り上げた商を返します。分母が0の場合の動作は未定義です。
/// @param _numerator 割られる値（分子）。
/// @param _denominator 割る値（分母）。0を渡してはなりません。
/// @return 切り上げた商をuint32_t型で返します（余りがあれば上方向に丸められる）。
uint32_t DivideAndRoundUp(uint32_t _numerator, uint32_t _denominator);

/// @brief _tを使い p0~p3 のCatmullRom補間位置を計算する
/// @param _p0 制御点0
/// @param _p1 制御点1
/// @param _p2 制御点2
/// @param _p3 制御点3
/// @param _t 補間パラメータ(0~1)
/// @return p0~p3のCatmullRom補間位置
Vector3 CatmullRomPosition(
	const Vector3& _p0, const Vector3& _p1,
	const Vector3& _p2, const Vector3& _p3,
	float _t
);


/// @brief pointが_minと_maxの内側にあるか判定する
/// @param _point 判定する点
/// @param _min Rect の最小値
/// @param _max Rect の最大値
/// @return true: 内側にある / false: 内側にない
bool Inside(const Vector2& _point, const Vector2& _min, const Vector2& _max);

/// @brief Vector3 を Vector4 に変換する
/// @param _v3 Vector3 型のベクトル
/// @param _w Vector4 の w 成分に設定する値
/// @return Vector4 型のベクトル
Vector4 ConvertToVector4(const Vector3& _v3, float _w);


} /// namespace Mathf

} /// namespace ONEngine