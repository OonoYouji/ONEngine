#pragma once

/// std
#include <cmath>

/// engine
#include "Vector3.h"
#include "Matrix4x4.h"

/// //////////////////////////////////////////////////
/// 三次元での回転を表すクラス
/// //////////////////////////////////////////////////
struct Quaternion final {
	/// ===================================================
	/// public : methods
	/// ===================================================

	/// @brief デフォルトコンストラクタ
	Quaternion();

	/// @brief パラメータ付きコンストラクタ
	/// @param _x x成分
	/// @param _y y成分
	/// @param _z z成分
	/// @param _w w成分
	Quaternion(float _x, float _y, float _z, float _w);

	/// ===================================================
	/// public : objects
	/// ===================================================

	float x, y, z, w;

	static const Quaternion kIdentity; ///< 単位クォータニオン

	/// ===================================================
	/// public : static methods
	/// ===================================================

	/// @brief クォータニオンの長さを計算する
	/// @param _q クォータニオン
	/// @return クォータニオンの長さ
	static float Length(const Quaternion& _q);

	/// @brief クォータニオンを正規化する
	/// @param _q クォータニオン
	/// @return 正規化されたクォータニオン
	static Quaternion Normalize(const Quaternion& _q);

	/// @brief ベクトルをクォータニオンで変換する
	/// @param _v ベクトル
	/// @param _q クォータニオン
	/// @return 変換されたベクトル
	static Vector3 Transform(const Vector3& _v, const Quaternion& _q);

	/// @brief クォータニオンを線形補間する
	/// @param _start 開始クォータニオン
	/// @param _end 終了クォータニオン
	/// @param _t 補間係数
	/// @return 補間されたクォータニオン
	static Quaternion Lerp(const Quaternion& _start, const Quaternion& _end, float _t);

	/// @brief ある軸を基にクォータニオンを計算する
	/// @param _axis 回転の軸となるベクトル
	/// @param _theta 回転角度
	/// @return 軸を基に回転させたクォータニオン
	static Quaternion MakeFromAxis(const Vector3& _axis, float _theta);

	/// @brief 回転行列を生成する
	/// @param axis 回転軸
	/// @param theta 回転角度
	/// @return 回転行列
	static Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float theta);

	/// @brief 特定の方向を見るクォータニオンを生成する
	/// @param _position 現在の位置
	/// @param _target 目標位置
	/// @param _up 上方向ベクトル
	/// @return 生成されたクォータニオン
	static Quaternion LookAt(const Vector3& _position, const Vector3& _target, const Vector3& _up);

	/// @brief 特定の方向を見るクォータニオンを生成する
	/// @param _position 現在の位置
	/// @param _target 目標位置
	/// @return 生成されたクォータニオン
	static Quaternion LookAt(const Vector3& _position, const Vector3& _target);

	/// @brief クォータニオンを球面線形補間する
	/// @param _start 開始クォータニオン
	/// @param _end 終了クォータニオン
	/// @param _t 補間係数
	/// @return 補間されたクォータニオン
	static Quaternion Slerp(const Quaternion& _start, const Quaternion& _end, float _t);



	/// @brief Euler角からクォータニオンを作成
	/// @param _euler Vector3形式のオイラー角（ラジアン）
	/// @return 生成されたクォータニオン
	static Quaternion FromEuler(const Vector3& _euler);

	/// @brief QuaternionからEuler角を作成
	/// @param _q Quaternion形式の回転情報
	/// @return Vector3形式のオイラー角（ラジアン）
	static Vector3 ToEuler(const Quaternion& _q);


	/// @brief 行列からクォータニオンを作成
	/// @param _m Matrix4x4形式の行列
	/// @return Quaternion形式の回転情報
	static Quaternion FromRotationMatrix(const Matrix4x4& _m);



	/// ===================================================
	/// public : methods
	/// ===================================================

	/// @brief クォータニオンの共役を計算する
	/// @return 共役クォータニオン
	Quaternion Conjugate() const;

	/// @brief クォータニオンのノルムを計算する
	/// @return クォータニオンのノルム
	float Length() const;

	/// @brief 逆クォータニオンを計算する
	/// @return 逆クォータニオン
	Quaternion Inverse() const;

	/// ===================================================
	/// public : operator
	/// ===================================================

	inline Quaternion& operator*= (const Quaternion& _other);
};



/// ===================================================
/// quaternion operator
/// ===================================================

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
