#pragma once

/// engine
#include "Vector3T.h"

namespace Engine::Math {

constexpr float kPI = 3.1415926535f;

inline float ToRadians(float degrees) {
    return degrees * (kPI / 180.0f);
}

inline float ToDegrees(float radians) {
    return radians * (180.0f / kPI);
}

using Vector3    = Vector3T<float>;
using Vector3Int = Vector3T<int32_t>;

} /// namespace Engine::Math