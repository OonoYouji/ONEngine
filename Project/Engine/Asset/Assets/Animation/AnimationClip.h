#pragma once

/// std
#include <string>
#include <vector>
#include <variant>

/// engine
#include "../IAsset.h"
#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"

namespace ONEngine::Asset {

/// ///////////////////////////////////////////////////
/// アニメーションのキーフレームデータ
/// ///////////////////////////////////////////////////
struct AnimationKeyframe {
    float time;
    std::variant<float, Vector2, Vector3, Vector4> value;
    std::string interpolation = "Linear"; // Linear, Step, Cubic
};

/// ///////////////////////////////////////////////////
/// アニメーションのトラック（1つのプロパティに対する変化）
/// ///////////////////////////////////////////////////
struct AnimationTrack {
    std::string componentName; // "Transform", "MeshRenderer", etc.
    std::string propertyPath;  // "position.x", "material.uvTransform.offset"
    std::vector<AnimationKeyframe> keyframes;
};

/// ///////////////////////////////////////////////////
/// アニメーションクリップアセット
/// ///////////////////////////////////////////////////
class AnimationClip : public IAsset {
public:
    struct MetaData {
        // 現在は特になし
        void to_json(nlohmann::json& /*_j*/) const {}
        void from_json(const nlohmann::json& /*_j*/) {}
    };

    AnimationClip() = default;
    ~AnimationClip() override = default;

    std::string name;
    float duration = 0.0f; // 非推奨化し、endFrameベースに移行予定
    int startFrame = 0;
    int endFrame = 60;
    bool isLooping = false;
    std::vector<AnimationTrack> tracks;
};

} /// namespace ONEngine::Asset
