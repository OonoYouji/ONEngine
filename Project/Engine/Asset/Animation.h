#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "Engine/Core/Math/Math.h"

namespace Engine::Asset {

template<typename T>
struct KeyFrame {
    float time;
    T value;
};

using KeyFrameVector3 = KeyFrame<Math::Vector3>;
using KeyFrameQuaternion = KeyFrame<Math::Quaternion>;

struct NodeAnimation {
    std::vector<KeyFrameVector3> translate;
    std::vector<KeyFrameQuaternion> rotate;
    std::vector<KeyFrameVector3> scale;
};

struct Animation {
    std::string name;
    float duration;
    std::unordered_map<std::string, NodeAnimation> nodeAnimations;
};

} // namespace Engine::Asset
