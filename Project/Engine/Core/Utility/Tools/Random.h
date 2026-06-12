#pragma once  

#define NOMINMAX 

#include <random>  
#include <limits> 

#include "../Math/Vector2.h"  
#include "../Math/Vector3.h"  
#include "../Math/Vector4.h"  

namespace ONEngine {

class Random final {
public:
	static int Int(int _min, int _max);
	static int Int();

	static uint64_t UInt64(uint64_t _min, uint64_t _max);
	static uint64_t UInt64();

	static float Float(float _min, float _max);
	static float Float();

	static Vector2 Vec2(const Vector2& _min, const Vector2& _max);
	static Vector3 Vec3(const Vector3& _min, const Vector3& _max);
	static Vector4 Vec4(const Vector4& _min, const Vector4& _max);

	static Vector3 InsideUnitSphere();

private:
	static std::mt19937 generator_;
};

} /// ONEngine
