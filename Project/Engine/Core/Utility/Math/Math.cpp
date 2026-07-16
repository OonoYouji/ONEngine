#include "Math.h"

/// std
#include <filesystem>
#include <fstream>

/// engine
#include "Engine/Core/Utility/Utility.h"

/// namespaceの短縮
namespace fs = std::filesystem;

using namespace ONEngine;

/**
 * @brief 余接（コタンジェント）を計算します。
 * @param _t 角度（ラジアン）
 * @return コタンジェント値
 */
float Math::Cot(float _t) {
	/// 逆タンジェント
	return 1.0f / std::tan(_t);
}


/**
 * @brief 分子を分母で割り、余りがある場合は切り上げた商を返します。
 * @param _numerator 分子
 * @param _denominator 分母
 * @return 切り上げた商
 */
uint32_t Math::DivideAndRoundUp(uint32_t _numerator, uint32_t _denominator) {
	if (_denominator == 0) {
		return 0; // ゼロ除算防止
	}

	return (_numerator + _denominator - 1) / _denominator;
}


/**
 * @brief パラメータ _t に基づき、制御点 p0〜p3 から Catmull-Rom スプライン補間された位置を計算します。
 * @param _p0 制御点0
 * @param _p1 開始点となる制御点1
 * @param _p2 終了点となる制御点2
 * @param _p3 制御点3
 * @param _t 補間係数 (0.0 ~ 1.0)
 * @return 補間された座標を示すVector3
 */
Vector3 Math::CatmullRomPosition(const Vector3& _p0, const Vector3& _p1, const Vector3& _p2, const Vector3& _p3, float _t) {
	float t2 = _t * _t;
	float t3 = t2 * _t;

	return 0.5f * (
		(2.0f * _p1) +
		(-_p0 + _p2) * _t +
		(2.0f * _p0 - 5.0f * _p1 + 4.0f * _p2 - _p3) * t2 +
		(-_p0 + 3.0f * _p1 - 3.0f * _p2 + _p3) * t3);
}

/**
 * @brief 指定した点が、矩形の最小座標と最大座標の範囲内（内側）に存在するか判定します。
 * @param _point 判定対象の座標
 * @param _min 矩形の最小境界 (左下等)
 * @param _max 矩形の最大境界 (右上等)
 * @return 範囲内なら true、それ以外は false
 */
bool ONEngine::Math::Inside(const Vector2& _point, const Vector2& _min, const Vector2& _max) {
	/// 点が矩形の内側にあるか判定
	return (_point.x >= _min.x && _point.x <= _max.x
		&& _point.y >= _min.y && _point.y <= _max.y);
}

/**
 * @brief Vector3 を W値を指定した Vector4 に変換します。
 * @param _v3 変換元のVector3
 * @param _w Vector4の第4成分 (W値)
 * @return 変換後のVector4
 */
Vector4 ONEngine::Math::ConvertToVector4(const Vector3& _v3, float _w) {
	return Vector4(
		_v3.x,
		_v3.y,
		_v3.z,
		_w
	);
}
