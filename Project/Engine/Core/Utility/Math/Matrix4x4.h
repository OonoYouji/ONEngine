#pragma once

/// external
#include <DirectXMath.h>

/// math
#include "Vector3.h"
#include "Vector4.h"

using namespace DirectX;


/// //////////////////////////////////////////////////
/// 4x4行列クラス
/// //////////////////////////////////////////////////
namespace ONEngine {

/**
 * @struct Matrix4x4
 * @brief 3Dグラフィックス用の4x4行列構造体。アフィン変換やビュー投影行列の作成、DirectXMathとの連携を提供します。
 */
struct Matrix4x4 final {
	/// ===================================================
	/// public : constructer
	/// ===================================================

	/**
	 * @brief デフォルトコンストラクタ。単位行列で初期化します。
	 */
	Matrix4x4();

	/**
	 * @brief コピーコンストラクタ。
	 */
	Matrix4x4(const Matrix4x4& _matrix);

	/**
	 * @brief 2次元浮動小数点配列から行列を構築します。
	 */
	Matrix4x4(const float _matrix[4][4]);

	/**
	 * @brief 各要素を個別に指定して行列を構築します。
	 */
	Matrix4x4(
		float _m00, float _m01, float _m02, float _m03,
		float _m10, float _m11, float _m12, float _m13,
		float _m20, float _m21, float _m22, float _m23,
		float _m30, float _m31, float _m32, float _m33
	);



	/// ===================================================
	/// public : static methods
	/// ===================================================

	/**
	 * @brief 拡大縮小行列を作成します。
	 * @param _v X, Y, Z各軸の拡縮率
	 * @return 拡縮変換用のMatrix4x4オブジェクト
	 */
	static Matrix4x4 MakeScale(const Vector3& _v);

	/**
	 * @brief X軸回りの回転行列を作成します。
	 * @param _angle 回転角度（ラジアン）
	 * @return 回転変換用のMatrix4x4オブジェクト
	 */
	static Matrix4x4 MakeRotateX(float _angle);

	/**
	 * @brief Y軸回りの回転行列を作成します。
	 * @param _angle 回転角度（ラジアン）
	 * @return 回転変換用のMatrix4x4オブジェクト
	 */
	static Matrix4x4 MakeRotateY(float _angle);

	/**
	 * @brief Z軸回りの回転行列を作成します。
	 * @param _angle 回転角度（ラジアン）
	 * @return 回転変換用のMatrix4x4オブジェクト
	 */
	static Matrix4x4 MakeRotateZ(float _angle);

	/**
	 * @brief 3軸（ロール・ピッチ・ヨー）オイラー角指定から回転行列を作成します。
	 * @param _v X, Y, Z各軸の回転角（ラジアン）
	 * @return 回転変換用のMatrix4x4オブジェクト
	 */
	static Matrix4x4 MakeRotate(const Vector3& _v);
	static Matrix4x4 MakeRotate(const struct Quaternion& _q);

	/**
	 * @brief 平行移動行列を作成します。
	 * @param _v X, Y, Z各方向への移動量
	 * @return 平行移動用のMatrix4x4オブジェクト
	 */
	static Matrix4x4 MakeTranslate(const Vector3& _v);

	/**
	 * @brief 拡大縮小、回転、平行移動からアフィン変換行列を一度に作成します。
	 * @param _scale 拡縮スケール
	 * @param _rotation 回転角
	 * @param _translation 平行移動
	 * @return 合成アフィン変換用のMatrix4x4オブジェクト
	 */
	static Matrix4x4 MakeAffine(const Vector3& _scale, const Vector3& _rotation, const Vector3& _translation);

	/**
	 * @brief 行列の転置行列（行と列を入れ替えた行列）を作成します。
	 * @param _matrix ソース行列
	 * @return 転置されたMatrix4x4オブジェクト
	 */
	static Matrix4x4 MakeTranspose(const Matrix4x4& _matrix);

	/**
	 * @brief 逆行列を計算して作成します。
	 * @param _matrix ソース行列
	 * @return 計算された逆行列（逆行列が存在しない場合は単位行列）
	 */
	static Matrix4x4 MakeInverse(const Matrix4x4& _matrix);

	/**
	 * @brief 左手座標系（Left-Handed）のビュー変換行列を作成します。
	 * @param _eye カメラの位置
	 * @param _target カメラが向く注視点
	 * @param _up カメラの上方向ベクトル
	 * @return ビュー変換行列
	 */
	static Matrix4x4 MakeLookAtLH(const Vector3& _eye, const Vector3& _target, const Vector3& _up);

	/**
	 * @brief Vector3座標ベクトルに対して行列によるアフィン変換（平行移動を含む）を実行します。
	 * @param _v 変換対象のベクトル
	 * @param _m 変換行列
	 * @return 変換後のVector3オブジェクト
	 */
	static Vector3 Transform(const Vector3& _v, const Matrix4x4& _m);

	/**
	 * @brief Vector3方向ベクトルに対して行列による回転のみ（平行移動は無視）の変換を実行します。
	 * @param _v 変換対象の方向ベクトル
	 * @param _m 変換行列
	 * @return 変換後のVector3オブジェクト
	 */
	static Vector3 TransformNormal(const Vector3& _v, const Matrix4x4& _m);

	/**
	 * @brief Vector4ベクトルに対して行列によるアフィン変換を実行します。
	 * @param _v 変換対象のVector4ベクトル
	 * @param _m 変換行列
	 * @return 変換後のVector4オブジェクト
	 */
	static Vector4 Transform(const Vector4& _v, const Matrix4x4& _m);

	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief 自身の転置行列を取得します。
	 * @return 転置されたMatrix4x4オブジェクト
	 */
	Matrix4x4 Transpose() const;

	/**
	 * @brief 自身の逆行列を取得します。
	 * @return 逆行列Matrix4x4オブジェクト
	 */
	Matrix4x4 Inverse() const;

	/**
	 * @brief アフィン行列からスケール（拡大縮小）成分を抽出します。
	 * @return スケール情報を含むVector3
	 */
	Vector3 ExtractScale() const;

	/**
	 * @brief アフィン行列から回転成分（クォータニオン）を抽出します。
	 * @return 回転を表すQuaternion
	 */
	Quaternion ExtractRotation() const;

	/**
	 * @brief アフィン行列から平行移動成分を抽出します。
	 * @return 移動量を示すVector3
	 */
	Vector3 ExtractTranslation() const;


	/// ===================================================
	/// public : static objects
	/// ===================================================

	static const Matrix4x4 kIdentity; ///< 単位行列の静的インスタンス



	/// ===================================================
	/// public : objects
	/// ===================================================

	float m[4][4]; ///< 4x4二次元配列の実数格納領域



	/// ===================================================
	/// public : operators
	/// ===================================================

	/**
	 * @brief 代入演算子
	 */
	inline Matrix4x4& operator=(const Matrix4x4& _other);

	/**
	 * @brief 乗算代入演算子
	 */
	inline Matrix4x4& operator*=(const Matrix4x4& _other);

};


namespace {


	/**
	 * @brief DirectXMathのXMMATRIX型からONEngineのMatrix4x4型へ変換します。
	 * @param _matrix DirectXMathの行列
	 * @return 変換後のMatrix4x4
	 */
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

	/**
	 * @brief ONEngineのMatrix4x4型からDirectXMathのXMMATRIX型へ変換します。
	 * @param _matrix ソースMatrix4x4
	 * @return DirectXMathのXMMATRIX
	 */
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

inline Vector4 operator*(const Vector4& _v, const Matrix4x4& _m) {
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

} /// ONEngine
