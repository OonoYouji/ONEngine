#include "EasingFunction.h"
#include <cmath>

float pi = float(3.1415926f);

float LerpE(const float& start, const float& end, float t) { return (1.0f - t) * start + end * t; }

Vec3 LerpE(const Vec3& start, const Vec3& end, float t) {
	Vec3 result;
	result.x = (1.0f - t) * start.x + end.x * t;
	result.y = (1.0f - t) * start.y + end.y * t;
	result.z = (1.0f - t) * start.z + end.z * t;
	return result;
}

Vec2 LerpE(const Vec2& start, const Vec2& end, float t) {
	Vec2 result;
	result.x = (1.0f - t) * start.x + end.x * t;
	result.y = (1.0f - t) * start.y + end.y * t;

	return result;
}

Vec3 SLerpE(const Vec3& start, const Vec3& end, float t) {
	Vec3 Nstart = Vector3::Normalize(start);
	Vec3 Nend = Vector3::Normalize(end);
	// 内積を求める
	float dot = Vector3::Dot(Nstart, Nend);
	// 誤差により1.0fを超えるのを防ぐ
	if (dot > 1.0f) {
		dot = 1.0f;
	}
	// アークコサインでθの角度を求める
	float theta = std::acos(dot);
	// θの角度からsinθを求める
	float sinTheta = std::sin(theta);
	// サイン(θ(1-t))を求める
	float sinThetaFrom = std::sin((1 - t) * theta);
	// サインθtを求める
	float sinThetaTo = std::sin(t * theta);
	Vec3 NormalizeVector;
	if (sinTheta < 1.0e-5) {
		NormalizeVector = Nstart;
	} else {
		// 球面線形補間したベクトル(単位ベクトル)
		NormalizeVector.x = (sinThetaFrom * Nstart.x + sinThetaTo * Nend.x) / sinTheta;
		NormalizeVector.y = (sinThetaFrom * Nstart.y + sinThetaTo * Nend.y) / sinTheta;
		NormalizeVector.z = (sinThetaFrom * Nstart.z + sinThetaTo * Nend.z) / sinTheta;
	}
	// ベクトルの長さはstartとendの長さを線形補間
	float length1 = Vector3::Length(start);
	float length2 = Vector3::Length(end);
	// LerpEで補間ベクトルの長さを求める
	float length = LerpE(length1, length2, t);
	// 長さを反映
	return NormalizeVector * length;
}


float EaseInElasticAmplitude(float t, const float& totaltime, const float& amplitude, const float& period) {

	if (t <= 0.0f) {
		return 0.0f;
	}
	if (t >= totaltime) {
		return 0.0f;
	}
	float s = period / (2.0f * pi) * std::asinf(1.0f);
	t /= totaltime;

	return -amplitude * std::powf(2.0f, 10.0f * (t - 1.0f)) * std::sinf((t - 1.0f - s) * (2.0f * pi) / period);
}

float EaseOutElasticAmplitude(float t, float totaltime, float amplitude, float period) {
	if (t <= 0.0f)
		return 0.0f;
	if (t >= totaltime)
		return 0.0f;

	float s = period / (2.0f * pi) * std::asin(1.0f);
	t /= totaltime;

	return amplitude * std::pow(2.0f, -10.0f * t) * std::sin((t - s) * (2.0f * pi) / period);
}

float EaseInOutElasticAmplitude(float t, float totaltime, float amplitude, float period) {
	if (t <= 0.0f)
		return 0.0f;
	if (t >= totaltime)
		return 0.0f;

	float backPoint = 0.5f;
	t /= totaltime;

	if (t < backPoint) {
		return EaseOutElasticAmplitude(t, totaltime, amplitude, period);
	} else {

		return EaseInElasticAmplitude(t - backPoint, totaltime - backPoint, amplitude, period);
	}
}

template<typename T> T EaseAmplitudeScale(const T& initScale, const float& easeT, const float& totalTime, const float& amplitude, const float& period) {
	T newScale = initScale; // T型のnewScaleを宣言

	if constexpr (std::is_same<T, float>::value) {
		newScale = initScale + -EaseOutElasticAmplitude(easeT, totalTime, amplitude, period);
	} else if constexpr (std::is_same<T, Vec2>::value) {
		newScale.x = initScale.x + -EaseOutElasticAmplitude(easeT, totalTime, amplitude, period);
		newScale.y = initScale.y + EaseOutElasticAmplitude(easeT, totalTime, amplitude, period);
	} else if constexpr (std::is_same<T, Vec3>::value) {
		newScale.x = initScale.x + -EaseOutElasticAmplitude(easeT, totalTime, amplitude, period);
		newScale.y = initScale.y + EaseOutElasticAmplitude(easeT, totalTime, amplitude, period);
		newScale.z = initScale.z + -EaseOutElasticAmplitude(easeT, totalTime, amplitude, period);
	}

	return newScale;
}

// EaseInSine 関数
template<typename T> T EaseInSine(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::cosf((t * pi) / 2.0f);
	return LerpE(start, end, easeT);
}

// EaseOutSine 関数
template<typename T> T EaseOutSine(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = std::sinf((t * pi) / 2.0f);
	return LerpE(start, end, easeT);
}

// EaseInOutSine 関数
template<typename T> T EaseInOutSine(const T& start, const T& end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT = 0.5f * (1.0f - std::cosf(t * pi));
	return LerpE(start, end, easeT);
}

/// EaseInBack
template<typename T>
T EaseInBack(const T& start, const T& end, float x, float totalX) {
	const float s = 1.70158f; // オーバーシュートの量 (調整可能)
	float t = x / totalX;
	float easeT = t * t * ((s + 1) * t - s);
	return LerpE(start, end, easeT);
}

/// EaseOutBack
template<typename T>
T EaseOutBack(const T& start, const T& end, float x, float totalX) {
	const float s = 1.70158f; // オーバーシュートの量 (調整可能)
	float t = x / totalX - 1;
	float easeT = (t * t * ((s + 1) * t + s)) + 1;
	return LerpE(start, end, easeT);
}


/// EaseInOutBack
template<typename T>
T EaseInOutBack(const T& start, const T& end, float x, float totalX) {
	const float s = 1.70158f * 1.525f; // オーバーシュートの量 (調整可能)
	float t = x / (totalX / 2.0f);
	float easeT;

	if (t < 1) {
		easeT = 0.5f * (t * t * ((s + 1) * t - s));
	}
	else {
		t -= 2;
		easeT = 0.5f * ((t * t * ((s + 1) * t + s)) + 2);
	}

	return LerpE(start, end, easeT);
}

// EaseInQuint 関数
template<typename T> T EaseInQuint(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = t * t * t * t * t;
	return LerpE(start, end, easeT);
}

// EaseOutQuint 関数
template<typename T> T EaseOutQuint(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::powf(1.0f - t, 5);
	return LerpE(start, end, easeT);
}

// EaseInOutQuint 関数
template<typename T> T EaseInOutQuint(const T& start, const T& end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = 0.5f * t * t * t * t * t;
	} else {
		t -= 2.0f;
		easeT = 0.5f * (std::powf(t, 5) + 2.0f);
	}
	return LerpE(start, end, easeT);
}

// EaseInCirc 関数
template<typename T> T EaseInCirc(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::sqrtf(1.0f - std::powf(t, 2));
	return LerpE(start, end, easeT);
}

// EaseOutCirc 関数
template<typename T> T EaseOutCirc(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = std::sqrtf(1.0f - std::powf(t - 1.0f, 2));
	return LerpE(start, end, easeT);
}

// EaseInOutCirc 関数
template<typename T> T EaseInOutCirc(const T& start, const T& end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = -0.5f * (std::sqrtf(1.0f - std::powf(t, 2)) - 1.0f);
	} else {
		t -= 2.0f;
		easeT = 0.5f * (std::sqrtf(1.0f - std::powf(t, 2)) + 1.0f);
	}
	return LerpE(start, end, easeT);
}

// EaseInExpo 関数
template<typename T> T EaseInExpo(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = (t == 0.0f) ? 0.0f : std::powf(2.0f, 10.0f * (t - 1.0f));
	return LerpE(start, end, easeT);
}

// EaseOutExpo 関数
template<typename T> T EaseOutExpo(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = (t == 1.0f) ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * t);
	return LerpE(start, end, easeT);
}

// EaseInOutExpo 関数
template<typename T> T EaseInOutExpo(const T& start, const T& end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = 0.5f * std::powf(2.0f, 10.0f * (t - 1.0f));
	} else {
		t -= 1.0f;
		easeT = 0.5f * (2.0f - std::powf(2.0f, -10.0f * t));
	}
	return LerpE(start, end, easeT);
}

// EaseInCubic 関数
template<typename T> T EaseInCubic(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = t * t * t;
	return LerpE(start, end, easeT);
}

// EaseOutCubic 関数
template<typename T> T EaseOutCubic(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::powf(1.0f - t, 3);
	return LerpE(start, end, easeT);
}

// EaseInOutCubic 関数
template<typename T> T EaseInOutCubic(const T& start, const T& end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = 0.5f * t * t * t;
	} else {
		t -= 2.0f;
		easeT = 0.5f * (std::powf(t, 3) + 2.0f);
	}
	return LerpE(start, end, easeT);
}

// EaseInQuad 関数
template<typename T> T EaseInQuad(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = t * t;
	return LerpE(start, end, easeT);
}

// EaseOutQuad 関数
template<typename T> T EaseOutQuad(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - (1.0f - t) * (1.0f - t);
	return LerpE(start, end, easeT);
}

// EaseInOutQuad 関数
template<typename T> T EaseInOutQuad(const T& start, const T& end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = 0.5f * t * t;
	} else {
		t -= 1.0f;
		easeT = -0.5f * (t * (t - 2.0f) - 1.0f);
	}
	return LerpE(start, end, easeT);
}

// EaseInQuart 関数
template<typename T> T EaseInQuart(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = t * t * t * t;
	return LerpE(start, end, easeT);
}

// EaseOutQuart 関数
template<typename T> T EaseOutQuart(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::powf(1.0f - t, 4);
	return LerpE(start, end, easeT);
}

// EaseInOutQuart 関数
template<typename T> T EaseInOutQuart(const T& start, const T& end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = 0.5f * t * t * t * t;
	} else {
		t -= 2.0f;
		easeT = -0.5f * (std::powf(t, 4) - 2.0f);
	}
	return LerpE(start, end, easeT);
}

//バウンス補助関数
float BounceEaseOut(float x) {

	const float n1 = 7.5625f;
	const float d1 = 2.75f;
	float easeT = 0.0f;

	if (x < 1.0f / d1) {
		easeT = n1 * x * x;
	} else if (x < 2.0f / d1) {
		x -= 1.5f / d1;
		easeT = n1 * x * x + 0.75f;
	} else if (x < 2.5f / d1) {
		x -= 2.25f / d1;
		easeT = n1 * x * x + 0.9375f;
	} else {
		x -= 2.625f / d1;
		easeT = n1 * x * x + 0.984375f;
	}
	return easeT;
}

// EaseInBounce 関数
template<typename T> T EaseInBounce(const T& start, const T& end, float x, float totalX) {
	float t = 1.0f - (x / totalX);
	float easeT = 1.0f - BounceEaseOut(t);
	return LerpE(start, end, easeT);
}

// EaseOutBounce 関数
template<typename T> T EaseOutBounce(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = BounceEaseOut(t);
	return LerpE(start, end, easeT);
}

// EaseInOutBounce 関数
template<typename T> T EaseInOutBounce(const T& start, const T& end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = 0.5f * (1.0f - BounceEaseOut(1.0f - t));
	} else {
		t -= 1.0f;
		easeT = 0.5f * BounceEaseOut(t) + 0.5f;
	}
	return LerpE(start, end, easeT);
}



/// EaseInElastic
template<typename T>
T EaseInElastic(const T& start, const T& end, float x, float totalX) {
	if (x == 0) return start;
	if (x == totalX) return end;

	float t = x / totalX;
	float p = 0.3f; // period
	float s = p / 4.0f; // calculated shift
	float easeT = -std::pow(2.0f, 10.0f * (t - 1)) * std::sin((t - 1 - s) * (2 * pi) / p);

	return LerpE(start, end, easeT);
}

/// EaseOutElastic
template<typename T>
T EaseOutElastic(const T& start, const T& end, float x, float totalX) {
	if (x == 0) return start;
	if (x == totalX) return end;

	float t = x / totalX;
	float p = 0.3f; // period
	float s = p / 4.0f; // calculated shift
	float easeT = std::pow(2.0f, -10.0f * t) * std::sin((t - s) * (2 * pi) / p) + 1.0f;

	return LerpE(start, end, easeT);
}


/// EaseInOutElastic
template<typename T>
T EaseInOutElastic(const T& start, const T& end, float x, float totalX) {
	if (x == 0) return start;
	if (x == totalX) return end;

	float t = x / (totalX / 2.0f);
	float p = 0.45f; // period
	float s = p / 4.0f; // calculated shift
	float easeT;

	if (t < 1) {
		easeT = -0.5f * std::pow(2.0f, 10.0f * (t - 1)) * std::sin((t - 1 - s) * (2 * pi) / p);
	}
	else {
		t -= 1;
		easeT = std::pow(2.0f, -10.0f * t) * std::sin((t - s) * (2 * pi) / p) * 0.5f + 1.0f;
	}

	return LerpE(start, end, easeT);
}

//// イージングタイムコントール
//template void EaseTimeControl(float& t, const float& totalTime, const Vec3& start, const Vec3& end);
//template void EaseTimeControl(float& t, const float& totalTime, const Vec2& start, const Vec2& end);
//template void EaseTimeControl(float& t, const float& totalTime, const float& start, const float& end);
// ぷにぷに
template Vec3 EaseAmplitudeScale<Vec3>(const Vec3& initScale, const float& easeT, const float& easeTime, const float& amplitude, const float& period);
template Vec2 EaseAmplitudeScale<Vec2>(const Vec2& initScale, const float& easeT, const float& easeTime, const float& amplitude, const float& period);
template float EaseAmplitudeScale<float>(const float& initScale, const float& easeT, const float& easeTime, const float& amplitude, const float& period);
//*******************************************************************************************************************************************************************
// Sine**************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************

template Vec3 EaseInSine<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInSine<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInSine<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseOutSine<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseOutSine<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseOutSine<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseInOutSine<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInOutSine<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInOutSine<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseInBack(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInBack(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInBack(const float& start, const float& end, float x, float totalX);

template Vec3 EaseOutBack(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseOutBack(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseOutBack(const float& start, const float& end, float x, float totalX);

template Vec3 EaseInOutBack(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInOutBack(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInOutBack(const float& start, const float& end, float x, float totalX);

template Vec3 EaseInQuint<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInQuint<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInQuint<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseOutQuint<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseOutQuint<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseOutQuint<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseInOutQuint<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInOutQuint<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInOutQuint<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseInCirc<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInCirc<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInCirc<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseOutCirc<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseOutCirc<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseOutCirc<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseInOutCirc<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInOutCirc<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInOutCirc<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseInExpo<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInExpo<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInExpo<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseOutExpo<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseOutExpo<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseOutExpo<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseInOutExpo<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInOutExpo<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInOutExpo<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseOutCubic<Vec3>(const Vec3& start, const Vec3& end, float x, float totalx);
template Vec2 EaseOutCubic<Vec2>(const Vec2& start, const Vec2& end, float x, float totalx);
template float EaseOutCubic<float>(const float& start, const float& end, float x, float totalx);

template Vec3 EaseInCubic<Vec3>(const Vec3& start, const Vec3& end, float x, float totalx);
template Vec2 EaseInCubic<Vec2>(const Vec2& start, const Vec2& end, float x, float totalx);
template float EaseInCubic<float>(const float& start, const float& end, float x, float totalx);

template Vec3 EaseInOutCubic<Vec3>(const Vec3& start, const Vec3& end, float x, float totalx);
template Vec2 EaseInOutCubic<Vec2>(const Vec2& start, const Vec2& end, float x, float totalx);
template float EaseInOutCubic<float>(const float& start, const float& end, float x, float totalx);

template Vec3 EaseInQuad<Vec3>(const Vec3& start, const Vec3& end, float x, float totalx);
template Vec2 EaseInQuad<Vec2>(const Vec2& start, const Vec2& end, float x, float totalx);
template float EaseInQuad<float>(const float& start, const float& end, float x, float totalx);

template Vec3 EaseOutQuad<Vec3>(const Vec3& start, const Vec3& end, float x, float totalx);
template Vec2 EaseOutQuad<Vec2>(const Vec2& start, const Vec2& end, float x, float totalx);
template float EaseOutQuad<float>(const float& start, const float& end, float x, float totalx);

template Vec3 EaseInOutQuad<Vec3>(const Vec3& start, const Vec3& end, float x, float totalx);
template Vec2 EaseInOutQuad<Vec2>(const Vec2& start, const Vec2& end, float x, float totalx);
template float EaseInOutQuad<float>(const float& start, const float& end, float x, float totalx);

template Vec3 EaseInQuart<Vec3>(const Vec3& start, const Vec3& end, float x, float totalx);
template Vec2 EaseInQuart<Vec2>(const Vec2& start, const Vec2& end, float x, float totalx);
template float EaseInQuart<float>(const float& start, const float& end, float x, float totalx);

template Vec3 EaseOutQuart<Vec3>(const Vec3& start, const Vec3& end, float x, float totalx);
template Vec2 EaseOutQuart<Vec2>(const Vec2& start, const Vec2& end, float x, float totalx);
template float EaseOutQuart<float>(const float& start, const float& end, float x, float totalx);

template Vec3 EaseInBounce<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInBounce<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInBounce<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseOutBounce<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseOutBounce<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseOutBounce<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseInOutBounce<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInOutBounce<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInOutBounce<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseInElastic<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInElastic<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInElastic<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseOutElastic<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseOutElastic<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseOutElastic<float>(const float& start, const float& end, float x, float totalX);

template Vec3 EaseInOutElastic<Vec3>(const Vec3& start, const Vec3& end, float x, float totalX);
template Vec2 EaseInOutElastic<Vec2>(const Vec2& start, const Vec2& end, float x, float totalX);
template float EaseInOutElastic<float>(const float& start, const float& end, float x, float totalX);