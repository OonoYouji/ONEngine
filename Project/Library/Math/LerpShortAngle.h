#pragma once

#include <cmath>
#include <numbers>

float LerpShortAngle(float a,float b,float t){
	constexpr float maxPi = std::numbers::pi_v<float> *2.0f;
	constexpr float minPi = std::numbers::pi_v<float> *-2.0f;

	float diff = b - a;

	float rotate = std::fmod(diff,maxPi);

	if(rotate >= std::numbers::pi){
		rotate += minPi;
	} else if(rotate <= -std::numbers::pi){
		rotate += maxPi;
	}

	return a + rotate * t;
}