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

	inline operator Vector4() const { return { r, g, b, a }; } ///< Vector4に変換

	void SetHSVtoRGB(float _h, float _s, float _v) {
		Vector4 color = HSVtoRGB(_h, _s, _v);
		r = color.x;
		g = color.y;
		b = color.z;
		a = color.w;
	}

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

	inline Color& operator=(const Vector4& _color);
	inline Color& operator=(const Color& _color);

};


inline Color::Color(uint32_t _colorCode) {
	SetColorCode(_colorCode);
}

inline void Color::SetColorCode(uint32_t _colorCode) {
	r = static_cast<float>((_colorCode >> 24) & 0xFF) / 255.0f;
	g = static_cast<float>((_colorCode >> 16) & 0xFF) / 255.0f;
	b = static_cast<float>((_colorCode >> 8)  & 0xFF) / 255.0f;
	a = static_cast<float>((_colorCode >> 0)  & 0xFF) / 255.0f;
}

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



class HSVColor final {
public:
	/// ===================================================  
	/// public : methods  
	/// ===================================================  
	HSVColor() = default;
	HSVColor(float _h, float _s, float _v) : h(_h), s(_s), v(_v) {}
	~HSVColor() = default;




public:
	/// ===================================================  
	/// public : objects  
	/// ===================================================  

	float h = 0.0f; ///< 色相
	float s = 0.0f; ///< 彩度
	float v = 0.0f; ///< 明度

};



//namespace ColorMethods {
//	/// @brief 色相を取得する
//	/// @param _color 色
//	/// @return 色相
//	inline float GetHue(const Color& _color) {
//		return std::atan2(_color.g - _color.b, _color.r - _color.g);
//	}
//	/// @brief 彩度を取得する
//	/// @param _color 色
//	/// @return 彩度
//	inline float GetSaturation(const Color& _color) {
//		return std::sqrt(_color.r * _color.r + _color.g * _color.g + _color.b * _color.b);
//	}
//}