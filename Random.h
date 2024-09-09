#pragma once

#include <random>

#include <Vector3.h>

class Random final {
	Random() = default;
	~Random() = default;
public:

	static int Int(int min, int max) {
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(generator_);
	}

	static float Float(float min, float max) {
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(generator_);
	}

	static Vec3 Vec3(const Vec3& min, const Vec3& max) {
		return {
			Float(min.x, max.x),
			Float(min.y, max.y),
			Float(min.z, max.z)
		};
	}

private:

	static std::mt19937 generator_;

};