#include "Random.h"
#include <algorithm>

using namespace ONEngine;

std::mt19937 Random::generator_(std::random_device{}());

int Random::Int(int _min, int _max) {
	std::uniform_int_distribution<int> distribution(_min, _max);
	return distribution(generator_);
}

int Random::Int() {
	return Int((std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)());
}

uint64_t Random::UInt64(uint64_t _min, uint64_t _max) {
	std::uniform_int_distribution<uint64_t> distribution(_min, _max);
	return distribution(generator_);
}

uint64_t Random::UInt64() {
	return UInt64((std::numeric_limits<uint64_t>::min)(), (std::numeric_limits<uint64_t>::max)());
}

float Random::Float(float _min, float _max) {
	if (_min > _max) {
		std::swap(_min, _max);
	}
	std::uniform_real_distribution<float> distribution(_min, _max);
	return distribution(generator_);
}

float Random::Float() {
	return Float((std::numeric_limits<float>::min)(), (std::numeric_limits<float>::max)());
}

Vector2 Random::Vec2(const Vector2& _min, const Vector2& _max) {
	return Vector2(
		Float(_min.x, _max.x),
		Float(_min.y, _max.y)
	);
}

Vector3 Random::Vec3(const Vector3& _min, const Vector3& _max) {
	return Vector3(
		Float(_min.x, _max.x),
		Float(_min.y, _max.y),
		Float(_min.z, _max.z)
	);
}

Vector4 Random::Vec4(const Vector4& _min, const Vector4& _max) {
	return Vector4(
		Float(_min.x, _max.x),
		Float(_min.y, _max.y),
		Float(_min.z, _max.z),
		Float(_min.w, _max.w)
	);
}

Vector3 Random::InsideUnitSphere() {
	while (true) {
		Vector3 p = Vector3(Float(-1, 1), Float(-1, 1), Float(-1, 1));
		if (p.LengthSquared() < 1.0f) return p;
	}
}
