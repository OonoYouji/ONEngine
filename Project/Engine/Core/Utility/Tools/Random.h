#pragma once  

#define NOMINMAX 

#include <random>  
#include <limits> 

#include "../Math/Vector2.h"  
#include "../Math/Vector3.h"  
#include "../Math/Vector4.h"  

namespace ONEngine {

/**
 * @class Random
 * @brief メルセンヌ・ツイスタ（std::mt19937）を用いた各種乱数生成ユーティリティを提供する静的クラス
 */
class Random final {
public:
	/**
	 * @brief 範囲 [min, max] 内のランダムな整数を生成します。
	 * @param _min 最小値（境界含む）
	 * @param _max 最大値（境界含む）
	 * @return 生成された整数
	 */
	static int Int(int _min, int _max);

	/**
	 * @brief 符号あり整数の全範囲からランダムな整数を生成します。
	 * @return 生成された整数
	 */
	static int Int();

	/**
	 * @brief 範囲 [min, max] 内のランダムな64ビット無符号整数を生成します。
	 * @param _min 最小値
	 * @param _max 最大値
	 * @return 生成されたuint64_t値
	 */
	static uint64_t UInt64(uint64_t _min, uint64_t _max);

	/**
	 * @brief 64ビット無符号整数の全範囲からランダムな数値を生成します。
	 * @return 生成されたuint64_t値
	 */
	static uint64_t UInt64();

	/**
	 * @brief 範囲 [min, max] 内のランダムな単精度浮動小数点数を生成します。
	 * @param _min 最小値
	 * @param _max 最大値
	 * @return 生成されたfloat値
	 */
	static float Float(float _min, float _max);

	/**
	 * @brief 範囲 [0.0, 1.0] 内のランダムな単精度浮動小数点数を生成します。
	 * @return 生成されたfloat値
	 */
	static float Float();

	/**
	 * @brief 指定した最小・最大ベクトルの各軸範囲内でランダムな Vector2 ベクトルを生成します。
	 * @param _min 最小境界ベクトル
	 * @param _max 最大境界ベクトル
	 * @return 生成されたVector2
	 */
	static Vector2 Vec2(const Vector2& _min, const Vector2& _max);

	/**
	 * @brief 指定した最小・最大ベクトルの各軸範囲内でランダムな Vector3 ベクトルを生成します。
	 * @param _min 最小境界ベクトル
	 * @param _max 最大境界ベクトル
	 * @return 生成されたVector3
	 */
	static Vector3 Vec3(const Vector3& _min, const Vector3& _max);

	/**
	 * @brief 指定した最小・最大ベクトルの各軸範囲内でランダムな Vector4 ベクトルを生成します。
	 * @param _min 最小境界ベクトル
	 * @param _max 最大境界ベクトル
	 * @return 生成されたVector4
	 */
	static Vector4 Vec4(const Vector4& _min, const Vector4& _max);

	/**
	 * @brief 半径 1.0 の単位球体の内部にあるランダムな座標を生成します。
	 * @return 球内のランダムなVector3座標
	 */
	static Vector3 InsideUnitSphere();

private:
	static std::mt19937 generator_; ///< 乱数ジェネレータ
};

} /// ONEngine
