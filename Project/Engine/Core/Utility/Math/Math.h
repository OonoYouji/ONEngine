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


/**
 * @brief 余接（コタンジェント）を計算します。
 * @param _t 角度（ラジアン）
 * @return コタンジェント値
 */
float Cot(float _t);


/**
 * @brief 分子を分母で割り、余りがある場合は切り上げた商を返します。分母が0の場合の動作は未定義です。
 * @param _numerator 割られる値（分子）。
 * @param _denominator 割る値（分母）。0を渡してはなりません。
 * @return 切り上げた商（余りがあれば上方向に丸められる）
 */
uint32_t DivideAndRoundUp(uint32_t _numerator, uint32_t _denominator);

/**
 * @brief パラメータ _t に基づき、制御点 p0〜p3 から Catmull-Rom スプライン補間された位置を計算します。
 * @param _p0 制御点0
 * @param _p1 開始点となる制御点1
 * @param _p2 終了点となる制御点2
 * @param _p3 制御点3
 * @param _t 補間係数 (0.0 ~ 1.0)
 * @return 補間された座標を示すVector3
 */
Vector3 CatmullRomPosition(
	const Vector3& _p0, const Vector3& _p1,
	const Vector3& _p2, const Vector3& _p3,
	float _t
);


/**
 * @brief 指定した点が、矩形の最小座標と最大座標の範囲内（内側）に存在するか判定します。
 * @param _point 判定対象の座標
 * @param _min 矩形の最小境界 (左下等)
 * @param _max 矩形の最大境界 (右上等)
 * @return 範囲内なら true、それ以外は false
 */
bool Inside(const Vector2& _point, const Vector2& _min, const Vector2& _max);

/**
 * @brief Vector3 を W値を指定した Vector4 に変換します。
 * @param _v3 変換元のVector3
 * @param _w Vector4の第4成分 (W値)
 * @return 変換後のVector4
 */
Vector4 ConvertToVector4(const Vector3& _v3, float _w);


} /// namespace Math

} /// namespace ONEngine