#pragma once  

#define NOMINMAX // Windowsのmin/maxマクロを無効化  

/// std  
#include <random>  
#include <limits> // std::numeric_limitsを使用するために必要  

/// engine  
#include "../Math/Vector2.h"  
#include "../Math/Vector3.h"  
#include "../Math/Vector4.h"  

class Random final {
	Random() = default;
	~Random() = default;
public:

	/// @brief int型のランダム値を得る  
	/// @param _min 最小値  
	/// @param _max 最大値  
	/// @return ランダムな値  
	static int Int(int _min, int _max) {
		std::uniform_int_distribution<int> distribution(_min, _max);
		return distribution(generator_);
	}

	/// @brief int型の最小値、最大値からランダムな値を得る  
	/// @return 最小値～最大値からランダムな値  
	static int Int() {
		return Int((std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)());
	}

	/// @brief float型のランダムな値を得る  
	/// @param _min 最小値  
	/// @param _max 最大値  
	/// @return _min ~ _maxの値でランダムな値を得る  
	static float Float(float _min, float _max) {
		if (_min > _max) {
			std::swap(_min, _max);
		}

		std::uniform_real_distribution<float> distribution(_min, _max);
		return distribution(generator_);
	}

	/// @brief float型の最小値、最大値からランダムな値を得る  
	/// @return 最小値～最大値からランダムな値  
	static float Float() {
		return Float((std::numeric_limits<float>::min)(), (std::numeric_limits<float>::max)());
	}

	static Vector2 Vector2(const Vector2& _min, const Vector2& _max) {
		return {
			Float(_min.x, _max.x),
			Float(_min.y, _max.y)
		};
	}

	static Vector3 Vector3(const Vector3& _min, const Vector3& _max) {
		return {
			Float(_min.x, _max.x),
			Float(_min.y, _max.y),
			Float(_min.z, _max.z)
		};
	}

	static Vector4 Vector4(const Vector4& _min, const Vector4& _max) {
		return {
			Float(_min.x, _max.x),
			Float(_min.y, _max.y),
			Float(_min.z, _max.z),
			Float(_min.w, _max.w)
		};
	}

private:

	static std::mt19937 generator_;

};