#pragma once

/// std
#include <cstdint>

/// engine
#include "Vector4.h"

/// ///////////////////////////////////////////////////
/// Colorクラス
/// ///////////////////////////////////////////////////
class Color final {
public:
	/// ===================================================  
	/// public : static methods, objects
	/// ===================================================  

	/// @brief HSVからRGBに変換する
	/// @param _h 色相
	/// @param _s 彩度
	/// @param _v 明度
	/// @return RGB
	static Vector4 HSVtoRGB(float _h, float _s, float _v);

	static const Color kWhite; ///< 白
	static const Color kBlack; ///< 黒
	static const Color kRed;   ///< 赤
	static const Color kGreen; ///< 緑
	static const Color kBlue;  ///< 青

public:
	/// ===================================================  
	/// public : methods  
	/// ===================================================  
	Color() = default;
	Color(const Vector4& _color) : r(_color.x), g(_color.y), b(_color.z), a(_color.w) {}
	Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
	Color(uint32_t _colorCode);
	~Color() = default;


	/// @brief HSVからRGBに変換してセットする
	/// @param _h 色相
	/// @param _s 彩度
	/// @param _v 明度
	void SetHSVtoRGB(float _h, float _s, float _v);

	/// @brief カラーコードからRGBAに変換してセットする
	/// @param _colorCode カラーコード 
	void SetColorCode(uint32_t _colorCode);

public:
	/// ===================================================  
	/// public : objects  
	/// ===================================================  

	float r; ///< 赤
	float g; ///< 緑
	float b; ///< 青
	float a; ///< アルファ


public:
	/// ===================================================  
	/// public : operator  
	/// ===================================================  

	inline operator Vector4() const { return { r, g, b, a }; } ///< Vector4に変換

	inline Color& operator=(const Vector4& _color);
	inline Color& operator=(const Color& _color);

};

inline Color& Color::operator=(const Vector4& _color) {
	r = _color.x;
	g = _color.y;
	b = _color.z;
	a = _color.w;
	return *this;
}

inline Color& Color::operator=(const Color& _color) {
	r = _color.r;
	g = _color.g;
	b = _color.b;
	a = _color.a;
	return *this;
}
