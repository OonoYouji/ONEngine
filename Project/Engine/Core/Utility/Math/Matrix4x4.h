#pragma once

/// external
#include <DirectXMath.h>

/// math
#include "Vector3.h"

using namespace DirectX;


/// //////////////////////////////////////////////////
/// 4x4行列クラス
/// //////////////////////////////////////////////////
struct Matrix4x4 final {
	/// ===================================================
	/// public : constructer
	/// ===================================================

	Matrix4x4();
	Matrix4x4(const Matrix4x4& _matrix);
	Matrix4x4(const float _matrix[4][4]);
	Matrix4x4(
		float _m00, float _m01, float _m02, float _m03,
		float _m10, float _m11, float _m12, float _m13,
		float _m20, float _m21, float _m22, float _m23,
		float _m30, float _m31, float _m32, float _m33
	);



	/// ===================================================
	/// public : static methods
	/// ===================================================

	/// @brief 拡縮行列の作成
	/// @param _v 拡縮度
	/// @return 拡縮行列
	static Matrix4x4 MakeScale(const Vector3& _v);

	/// @brief 回転行列の作成 X軸
	/// @param _angle 回転角
	/// @return 回転行列
	static Matrix4x4 MakeRotateX(float _angle);

	/// @brief 回転行列の作成 Y軸
	/// @param _angle 回転角
	/// @return 回転行列
	static Matrix4x4 MakeRotateY(float _angle);

	/// @brief 回転行列の作成 Z軸
	/// @param _angle 回転角
	/// @return 回転行列
	static Matrix4x4 MakeRotateZ(float _angle);

	/// @brief 回転行列の作成
	/// @param _v 回転率
	/// @return 回転行列
	static Matrix4x4 MakeRotate(const Vector3& _v);
	static Matrix4x4 MakeRotate(const class Quaternion& _q);

	/// @brief 平行移動行列の作成
	/// @param _v 平行移動成分
	/// @return 平行移動行列
	static Matrix4x4 MakeTranslate(const Vector3& _v);

	/// @brief アフィン行列の作成
	/// @param _scale 拡縮度
	/// @param _rotation 回転率
	/// @param _translation 平行移動成分
	/// @return アフィン行列
	static Matrix4x4 MakeAffine(const Vector3& _scale, const Vector3& _rotation, const Vector3& _translation);

	/// @brief 転置行列の作成
	/// @param _matrix 他の行列
	/// @return 転置行列
	static Matrix4x4 MakeTranspose(const Matrix4x4& _matrix);

	/// @brief 逆行列の作成
	/// @param _matrix 他の行列
	/// @return 逆行列
	static Matrix4x4 MakeInverse(const Matrix4x4& _matrix);

	/// @brief ベクトルに行列をかける
	/// @param _v ベクトル
	/// @param _m 行列
	/// @return 変換後のベクトル
	static Vector3 Transform(const Vector3& _v, const Matrix4x4& _m);


	/// ===================================================
	/// public : methods
	/// ===================================================

	/// @brief 転置行列の取得
	/// @return 転置行列
	Matrix4x4 Transpose() const;

	/// @brief 逆行列の取得
	/// @return 逆行列
	Matrix4x4 Inverse() const;

	/// @brief 行列から拡縮成分を抽出する
	Vector3 ExtractScale() const;

	/// @brief 行列から回転成分を抽出する
	Quaternion ExtractRotation() const;

	/// @brief 行列から平行移動成分を抽出する
	Vector3 ExtractTranslation() const;


	/// ===================================================
	/// public : static objects
	/// ===================================================

	static const Matrix4x4 kIdentity; ///< 単位行列



	/// ===================================================
	/// public : objects
	/// ===================================================

	float m[4][4];



	/// ===================================================
	/// public : operators
	/// ===================================================

	/// @brief 代入演算子 operator
	/// @param _other 他行列
	/// @return 代入結果
	inline Matrix4x4& operator=(const Matrix4x4& _other);

	/// @brief 乗算代入演算子 operator
	/// @param _other 他行列
	/// @return 乗算代入結果
	inline Matrix4x4& operator*=(const Matrix4x4& _other);

};


namespace {
	inline Matrix4x4 Convert(const XMMATRIX& _matrix) {
		Matrix4x4  result;
		XMFLOAT4X4 tempMatrix;
		XMStoreFloat4x4(&tempMatrix, _matrix);

		for (size_t i = 0; i < 4; ++i) {
			for (size_t j = 0; j < 4; ++j) {
				result.m[i][j] = tempMatrix.m[i][j];
			}
		}
		return result;
	}

	inline XMMATRIX Convert(const Matrix4x4& _matrix) {
		return XMMATRIX(
			_matrix.m[0][0], _matrix.m[0][1], _matrix.m[0][2], _matrix.m[0][3],
			_matrix.m[1][0], _matrix.m[1][1], _matrix.m[1][2], _matrix.m[1][3],
			_matrix.m[2][0], _matrix.m[2][1], _matrix.m[2][2], _matrix.m[2][3],
			_matrix.m[3][0], _matrix.m[3][1], _matrix.m[3][2], _matrix.m[3][3]
		);
	}
}




/// ===================================================
/// operators
/// ===================================================

inline Matrix4x4 operator*(const Matrix4x4& _m1, const Matrix4x4& _m2) {
	return Convert(Convert(_m1) * Convert(_m2));
}

inline Vector3 operator*(const Vector3& _v, const Matrix4x4& _m) {
	return Matrix4x4::Transform(_v, _m);
}

/// ===================================================
/// public : operators
/// ===================================================

inline Matrix4x4& Matrix4x4::operator=(const Matrix4x4& _other) {
	for (size_t r = 0; r < 4; r++) {
		for (size_t c = 0; c < 4; c++) {
			m[r][c] = _other.m[r][c];
		}
	}
	return *this;
}

inline Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& _other) {
	*this = *this * _other;
	return *this;
}
