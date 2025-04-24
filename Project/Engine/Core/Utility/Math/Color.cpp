#include "Color.h"

/// std
#include <cmath> 

const Color Color::kWhite = Color(1.0f, 1.0f, 1.0f, 1.0f); ///< 白
const Color Color::kBlack = Color(0.0f, 0.0f, 0.0f, 1.0f); ///< 黒
const Color Color::kRed   = Color(1.0f, 0.0f, 0.0f, 1.0f); ///< 赤
const Color Color::kGreen = Color(0.0f, 1.0f, 0.0f, 1.0f); ///< 緑
const Color Color::kBlue  = Color(0.0f, 0.0f, 1.0f, 1.0f); ///< 青


Vector4 Color::HSVtoRGB(float _h, float _s, float _v) {
	float c = _v * _s;
	float x = c * (1 - std::fabsf(std::fmodf(_h / 60.0f, 2) - 1));
	float m = _v - c;
	float r, g, b;

	if (_h >= 0 && _h < 60) {
		r = c; g = x; b = 0;
	} else if (_h >= 60 && _h < 120) {
		r = x; g = c; b = 0;
	} else if (_h >= 120 && _h < 180) {
		r = 0; g = c; b = x;
	} else if (_h >= 180 && _h < 240) {
		r = 0; g = x; b = c;
	} else if (_h >= 240 && _h < 300) {
		r = x; g = 0; b = c;
	} else {
		r = c; g = 0; b = x;
	}

	return Vector4(r + m, g + m, b + m, 1.0f);
}
