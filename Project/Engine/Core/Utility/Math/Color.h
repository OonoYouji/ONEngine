#pragma once

/// std
#include <cstdint>

/// engine
#include "Vector4.h"

/// ///////////////////////////////////////////////////
/// Colorクラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @struct Color
 * @brief RGBA色情報を表現し、HSV変換やカラーコードのユーティリティを提供する構造体
 */
struct Color final {
	/// ===================================================  
	/// public : static methods, objects
	/// ===================================================  

	/**
	 * @brief HSVカラー空間の値をRGB（Vector4）に変換します。
	 * @param _h 色相 (0.0 ~ 360.0)
	 * @param _s 彩度 (0.0 ~ 1.0)
	 * @param _v 明度 (0.0 ~ 1.0)
	 * @return RGB（Alphaは1.0）のVector4ベクトル
	 */
	static Vector4 HSVtoRGB(float _h, float _s, float _v);

	static const Color kWhite; ///< 白色 (1.0, 1.0, 1.0, 1.0)
	static const Color kBlack; ///< 黒色 (0.0, 0.0, 0.0, 1.0)
	static const Color kRed;   ///< 赤色 (1.0, 0.0, 0.0, 1.0)
	static const Color kGreen; ///< 緑色 (0.0, 1.0, 0.0, 1.0)
	static const Color kBlue;  ///< 青色 (0.0, 0.0, 1.0, 1.0)

	/// ===================================================  
	/// public : methods  
	/// ===================================================  

	/**
	 * @brief デフォルトコンストラクタ
	 */
	Color() = default;

	/**
	 * @brief Vector4ベクトルから色を初期化するコンストラクタ
	 * @param _color RGBA情報を含むベクトル
	 */
	Color(const Vector4& _color) : r(_color.x), g(_color.y), b(_color.z), a(_color.w) {}

	/**
	 * @brief RGBA浮動小数点値を直接指定するコンストラクタ
	 * @param _r 赤 (0.0 ~ 1.0)
	 * @param _g 緑 (0.0 ~ 1.0)
	 * @param _b 青 (0.0 ~ 1.0)
	 * @param _a アルファ (0.0 ~ 1.0)
	 */
	Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}

	/**
	 * @brief 32ビットカラーコード（例: 0xFFFFFFFF）から色を初期化するコンストラクタ
	 * @param _colorCode RGBAカラーコード
	 */
	Color(uint32_t _colorCode);

	/**
	 * @brief デストラクタ
	 */
	~Color() = default;


	/**
	 * @brief HSV値をRGB値に変換し、自身のRGBA値にセットします（Alphaは1.0）。
	 * @param _h 色相 (0.0 ~ 360.0)
	 * @param _s 彩度 (0.0 ~ 1.0)
	 * @param _v 明度 (0.0 ~ 1.0)
	 */
	void SetHSVtoRGB(float _h, float _s, float _v);

	/**
	 * @brief 32ビットカラーコード（RGBA）からRGBAに展開し、自身にセットします。
	 * @param _colorCode RGBAカラーコード
	 */
	void SetColorCode(uint32_t _colorCode);

	/// ===================================================  
	/// public : objects  
	/// ===================================================  

	float r; ///< 赤チャンネル (0.0 ~ 1.0)
	float g; ///< 緑チャンネル (0.0 ~ 1.0)
	float b; ///< 青チャンネル (0.0 ~ 1.0)
	float a; ///< アルファチャンネル (0.0 ~ 1.0)


	/// ===================================================  
	/// public : operator  
	/// ===================================================  

	/**
	 * @brief Vector4への暗黙の型変換演算子
	 */
	inline operator Vector4() const { return { r, g, b, a }; }

};

} /// ONEngine
