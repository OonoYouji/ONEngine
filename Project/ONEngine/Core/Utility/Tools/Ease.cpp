#include "Ease.h"

/// std
#include <cmath>

#include "Engine/Core/Utility/Math/Math.h"

namespace ONEngine {
namespace Ease {

namespace In {
float Sine(float t) {
	return 1.0f - std::cos((t * Math::PI) / 2.0f);
}
float Quad(float t) {
	return t * t;
}
float Cubic(float t) {
	return t * t * t;
}
float Quart(float t) {
	return t * t * t * t;
}
float Quint(float t) {
	return t * t * t * t * t;
}
float Expo(float t) {
	return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * t - 10.0f);
}
float Circ(float t) {
	return 1.0f - std::sqrt(1.0f - std::pow(t, 2.0f));
}
float Back(float t) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	return c3 * t * t * t - c1 * t * t;
}
float Elastic(float t) {
	const float c4 = (2.0f * Math::PI) / 3.0f;
	return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : -std::pow(2.0f, 10.0f * t - 10.0f) * std::sin((t * 10.0f - 10.75f) * c4);
}
float Bounce(float t) {
	return 1.0f - Out::Bounce(1.0f - t);
}
} /// namespace In

namespace Out {
float Sine(float t) {
	return std::sin((t * Math::PI) / 2.0f);
}
float Quad(float t) {
	return 1.0f - (1.0f - t) * (1.0f - t);
}
float Cubic(float t) {
	return 1.0f - std::pow(1.0f - t, 3.0f);
}
float Quart(float t) {
	return 1.0f - std::pow(1.0f - t, 4.0f);
}
float Quint(float t) {
	return 1.0f - std::pow(1.0f - t, 5.0f);
}
float Expo(float t) {
	return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
}
float Circ(float t) {
	return std::sqrt(1.0f - std::pow(t - 1.0f, 2.0f));
}
float Back(float t) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
}
float Elastic(float t) {
	const float c4 = (2.0f * Math::PI) / 3.0f;
	return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
}
float Bounce(float t) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if(t < 1.0f / d1) {
		return n1 * t * t;
	} else if(t < 2.0f / d1) {
		t -= 1.5f / d1;
		return n1 * t * t + 0.75f;
	} else if(t < 2.5f / d1) {
		t -= 2.25f / d1;
		return n1 * t * t + 0.9375f;
	} else {
		t -= 2.625f / d1;
		return n1 * t * t + 0.984375f;
	}
}
} /// namespace Out

namespace InOut {
float Sine(float t) {
	return -(std::cos(Math::PI * t) - 1.0f) / 2.0f;
}
float Quad(float t) {
	return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}
float Cubic(float t) {
	return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}
float Quart(float t) {
	return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
}
float Quint(float t) {
	return t < 0.5f ? 16.0f * t * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 5.0f) / 2.0f;
}
float Expo(float t) {
	return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : t < 0.5f ? std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f : (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
}
float Circ(float t) {
	return t < 0.5f ? (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) / 2.0f : (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
}
float Back(float t) {
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;
	return t < 0.5f ? (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f : (std::pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
}
float Elastic(float t) {
	const float c5 = (2.0f * Math::PI) / 4.5f;
	return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : t < 0.5f ? -(std::pow(2.0f, 20.0f * t - 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f : (std::pow(2.0f, -20.0f * t + 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
}
float Bounce(float t) {
	return t < 0.5f ? (1.0f - Out::Bounce(1.0f - 2.0f * t)) / 2.0f : (1.0f + Out::Bounce(2.0f * t - 1.0f)) / 2.0f;
}
} /// namespace InOut

} /// namespace Ease
} /// namespace ONEngine