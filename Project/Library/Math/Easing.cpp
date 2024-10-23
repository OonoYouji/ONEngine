#include "Easing.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

/// ====================================================
///	Ease In
/// ====================================================

float Ease::In::Sine(float t) {
	return 1.0f - cosf((t * static_cast<float>(M_PI)) / 2.0f);
}

float Ease::In::Cubic(float t) {
	return powf(t, 3);
}

float Ease::In::Quint(float t) {
	return powf(t, 5);
}

float Ease::In::Circ(float t) {
	return 1.0f - sqrtf(1.0f - powf(t, 2.0f));
}

float Ease::In::Elastic(float t) {
	const float c4 = (2.0f * static_cast<float>(M_PI)) / 3.0f;

	if(t == 0.0f) {
		return 0.0f;
	} else if(t == 1.0f) {
		return 1.0f;
	} else {
		return -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
	}
}

float Ease::In::Quad(float t) {
	return powf(t, 2.0f);
}

float Ease::In::Quart(float t) {
	return powf(t, 4.0f);
}

float Ease::In::Expo(float t) {
	if(t == 0.0f) {
		return 0.0f;
	} else {
		return powf(2.0f, 10.0f * t - 10.0f);
	};
}

float Ease::In::Back(float t) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return c3 * t * t * t - c1 * t * t;
}

float Ease::In::Bounce(float t) {
	return 1.0f - Ease::Out::Bounce(1.0f - t);
}


/// ====================================================
///	Ease Out
/// ====================================================

float Ease::Out::Sine(float t) {
	return sinf((t * static_cast<float>(M_PI)) / 2.0f);
}

float Ease::Out::Cubic(float t) {
	return 1.0f - powf(1.0f - t, 3.0f);
}

float Ease::Out::Quint(float t) {
	return 1.0f - powf(1.0f - t, 5.0f);
}

float Ease::Out::Circ(float t) {
	return sqrtf(1.0f - powf(t - 1.0f, 2.0f));
}

float Ease::Out::Elastic(float t) {
	const float c4 = (2.0f * static_cast<float>(M_PI)) / 3.0f;

	if(t == 0.0f) {
		return 0.0f;
	} else if(t == 1.0f) {
		return 1.0f;
	} else {
		return powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
	}
}

float Ease::Out::Quad(float t) {
	return 1.0f - (1.0f - t) * (1.0f - t);
}

float Ease::Out::Quart(float t) {
	return 1.0f - powf(1.0f - t, 4.0f);
}

float Ease::Out::Expo(float t) {
	if(t == 1.0f) {
		return 1.0f;
	} else {
		return 1.0f - powf(2.0f, -10.0f * t);
	}
}

float Ease::Out::Back(float t) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
}

float Ease::Out::Bounce(float t) {
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


/// ====================================================
///	Ease InOut
/// ====================================================

float Ease::InOut::Sine(float t) {
	return (-cosf(static_cast<float>(M_PI) * t) + 1.0f) / 2.0f;
}

float Ease::InOut::Cubic(float t) {
	if(t < 0.5f) {
		return 4.0f * t * t * t;
	} else {
		return 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
	}
}

float Ease::InOut::Quint(float t) {
	if(t < 0.5f) {
		return 16.0f * t * t * t * t * t;
	} else {
		return 1.0f - powf(-2.0f * t + 2.0f, 5.0f) / 2.0f;
	}
}

float Ease::InOut::Circ(float t) {
	if(t < 0.5f) {
		return (1.0f - sqrtf(1.0f - powf(2.0f * t, 2.0f))) / 2.0f;
	} else {
		return (sqrtf(1.0f - powf(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
	}
}

float Ease::InOut::Elastic(float t) {
	const float c5 = (2.0f * static_cast<float>(M_PI)) / 4.5f;

	if(t == 0.0f) {
		return 0.0f;
	} else if(t == 1.0f) {
		return 1.0f;
	} else if(t < 0.5f) {
		return -(powf(2.0f, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f;
	} else {
		return (powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
	}
}

float Ease::InOut::Quad(float t) {
	if(t < 0.5f) {
		return 2.0f * t * t;
	} else {
		return 1.0f - powf(-2.0f * t + 2.0f, 2.0f) / 2.0f;
	}
}

float Ease::InOut::Quart(float t) {
	if(t < 0.5f) {
		return 8.0f * t * t * t * t;
	} else {
		return 1.0f - powf(-2.0f * t + 2.0f, 4.0f) / 2.0f;
	}
}

float Ease::InOut::Expo(float t) {
	if(t == 0.0f) {
		return 0.0f;
	} else if(t == 1.0f) {
		return 1.0f;
	} else if(t < 0.5f) {
		return powf(2.0f, 20.0f * t - 10.0f) / 2.0f;
	} else {
		return (2.0f - powf(2.0f, -20.0f * t + 10.0f)) / 2.0f;
	}
}

float Ease::InOut::Back(float t) {
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	if(t < 0.5f) {
		return static_cast<float>((std::pow(2.0f * t, 2) * ((c2 + 1) * 2.0f * t - c2)) / 2.0f);
	} else {
		return static_cast<float>((std::pow(2.0f * t - 2.0f, 2) * ((c2 + 1) * (2.0f * t - 2.0f) + c2) + 2.0f) / 2.0f);
	}
}

float Ease::InOut::Bounce(float t) {
	if(t < 0.5f) {
		return (1.0f - Ease::Out::Bounce(1.0f - 2.0f * t)) / 2.0f;
	} else {
		return (1.0f + Ease::Out::Bounce(2.0f * t - 1.0f)) / 2.0f;
	}
}