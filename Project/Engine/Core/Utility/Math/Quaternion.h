#pragma once

/// std
#include <cmath>

/// engine
#include "Vector3.h"
#include "Matrix4x4.h"

/// //////////////////////////////////////////////////
/// 三次元での回転を表すクラス
/// //////////////////////////////////////////////////
namespace ONEngine {

/**
 * @struct Quaternion
 * @brief 3D空間の回転を表現するクォータニオン（四元数）構造体。ジンバルロックの回避や球面線形補間に用いられます。
 */

struct Quaternion final {
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief デフォルトコンストラクタ。単位クォータニオン (0, 0, 0, 1) で初期化します。
	 */
	Quaternion();

	/**
	 * @brief 各成分を個別に指定して初期化するコンストラクタ。
	 * @param _x 虚数部X
	 * @param _y 虚数部Y
	 * @param _z 虚数部Z
	 * @param _w 実数部W
	 */
	Quaternion(float _x, float _y, float _z, float _w);

	/// ===================================================
	/// public : objects
	/// ===================================================

	float x, y, z, w;

	static const Quaternion kIdentity; ///< 単位クォータニオンの静的インスタンス

	/// ===================================================
	/// public : static methods
	/// ===================================================

	/**
	 * @brief クォータニオンの長さ（ノルム）を取得します。
	 * @param _q 対象のクォータニオン
	 * @return クォータニオンの長さ
	 */
	static float Length(const Quaternion& _q);

	/**
	 * @brief クォータニオンの正規化を行います。
	 * @param _q 正規化するクォータニオン
	 * @return 正規化されたQuaternion。長さが0の場合は単位クォータニオンを返します。
	 */
	static Quaternion Normalize(const Quaternion& _q);

	/**
	 * @brief Vector3座標ベクトルをクォータニオン回転により変換します。
	 * @param _v 回転するベクトル
	 * @param _q 回転を表現するクォータニオン
	 * @return 回転後のVector3
	 */
	static Vector3 Transform(const Vector3& _v, const Quaternion& _q);

	/**
	 * @brief 2つのクォータニオンの線形補間（Lerp）を行います。
	 * @param _start 開始クォータニオン
	 * @param _end 終了クォータニオン
	 * @param _t 補間係数 (0.0 ~ 1.0)
	 * @return 補間されたクォータニオン
	 */
	static Quaternion Lerp(const Quaternion& _start, const Quaternion& _end, float _t);

	/**
	 * @brief 回転軸と回転角度からクォータニオンを作成します。
	 * @param _axis 回転の軸ベクトル（正規化されている必要があります）
	 * @param _theta 回転角度（ラジアン）
	 * @return 軸・角から構築されたQuaternion
	 */
	static Quaternion MakeFromAxis(const Vector3& _axis, float _theta);

	/**
	 * @brief 回転軸と回転角度から回転行列を作成します。
	 * @param axis 回転軸
	 * @param theta 回転角度（ラジアン）
	 * @return 回転行列Matrix4x4
	 */
	static Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float theta);

	/**
	 * @brief 視点・注視点・上方向ベクトルから、注視方向を向くクォータニオンを作成します。
	 * @param _position 現在のカメラ等の位置
	 * @param _target 目標位置
	 * @param _up 上方向を示すベクトル
	 * @return 作成された回転を示すQuaternion
	 */
	static Quaternion LookAt(const Vector3& _position, const Vector3& _target, const Vector3& _up);

	/**
	 * @brief 視点・注視点から、注視方向を向くクォータニオンを作成します（上方向は世界座標の上方向と仮定）。
	 * @param _position 現在の位置
	 * @param _target 目標位置
	 * @return 作成された回転を示すQuaternion
	 */
	static Quaternion LookAt(const Vector3& _position, const Vector3& _target);

	/**
	 * @brief 2つのクォータニオンの球面線形補間（Slerp）を行います。等速で滑らかな回転補間に適しています。
	 * @param _start 開始クォータニオン
	 * @param _end 終了クォータニオン
	 * @param _t 補間係数 (0.0 ~ 1.0)
	 * @return 球面線形補間されたクォータニオン
	 */
	static Quaternion Slerp(const Quaternion& _start, const Quaternion& _end, float _t);

	/**
	 * @brief 3軸のオイラー角からクォータニオンを作成します。
	 * @param _euler 各軸の回転角ベクトル（ラジアン）
	 * @return 作成されたQuaternion
	 */
	static Quaternion FromEuler(const Vector3& _euler);

	/**
	 * @brief クォータニオンからオイラー角を逆算して作成します。
	 * @param _q 回転を示すクォータニオン
	 * @return 3軸の回転オイラー角（ラジアン）ベクトル
	 */
	static Vector3 ToEuler(const Quaternion& _q);

	/**
	 * @brief 回転行列から回転成分を抽出したクォータニオンを作成します。
	 * @param _m 回転行列
	 * @return 作成されたQuaternion
	 */
	static Quaternion FromRotationMatrix(const Matrix4x4& _m);



	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief 共役クォータニオンを取得します。
	 * @return 共役されたQuaternion
	 */
	Quaternion Conjugate() const;

	/**
	 * @brief クォータニオンの長さ（ノルム）を取得します。
	 * @return クォータニオンの長さ
	 */
	float Length() const;

	/**
	 * @brief 逆クォータニオンを取得します。
	 * @return 逆Quaternion
	 */
	Quaternion Inverse() const;

	/**
	 * @brief 別のクォータニオンとの内積を計算します。
	 * @param _other 対象のクォータニオン
	 * @return 内積値
	 */
	float Dot(const Quaternion& _other) const;

	/// ===================================================
	/// public : operator
	/// ===================================================

	inline Quaternion& operator*= (const Quaternion& _other);
	inline Quaternion& operator+= (const Quaternion& _other);
};



/// ===================================================
/// quaternion operator
/// ===================================================

inline Quaternion operator+ (const Quaternion& _q1, const Quaternion& _q2) {
	return { _q1.x + _q2.x, _q1.y + _q2.y, _q1.z + _q2.z, _q1.w + _q2.w };
}

inline Quaternion operator* (const Quaternion& _q, float _f) {
	return { _q.x * _f, _q.y * _f, _q.z * _f, _q.w * _f };
}

inline Quaternion operator* (float _f, const Quaternion& _q) {
	return _q * _f;
}

inline Quaternion operator* (const Quaternion& _q1, const Quaternion& _q2) {
	return {
		_q1.w * _q2.x + _q1.x * _q2.w + _q1.y * _q2.z - _q1.z * _q2.y,
		_q1.w * _q2.y + _q1.y * _q2.w + _q1.z * _q2.x - _q1.x * _q2.z,
		_q1.w * _q2.z + _q1.z * _q2.w + _q1.x * _q2.y - _q1.y * _q2.x,
		_q1.w * _q2.w - _q1.x * _q2.x - _q1.y * _q2.y - _q1.z * _q2.z
	};
}

inline Quaternion operator/ (const Quaternion& _q, float _value) {
	Quaternion result;
	result.w = _q.w / _value;
	result.x = _q.x / _value;
	result.y = _q.y / _value;
	result.z = _q.z / _value;
	return result;
}


inline Quaternion& Quaternion::operator*=(const Quaternion& _other) {
	*this = *this * _other;
	return *this;
}

inline Quaternion& Quaternion::operator+=(const Quaternion& _other) {
	*this = *this + _other;
	return *this;
}

inline bool operator==(const Quaternion& _q1, const Quaternion& _q2) {
	return _q1.x == _q2.x && _q1.y == _q2.y && _q1.z == _q2.z && _q1.w == _q2.w;
}

inline bool operator!=(const Quaternion& _q1, const Quaternion& _q2) {
	return !(_q1 == _q2);
}

} /// ONEngine
