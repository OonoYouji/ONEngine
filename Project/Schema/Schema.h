#pragma once
#include "Engine/Core/Math/Math.h"
#include <cstdint>

namespace Engine::GeneratedSchema {

struct SceneData {
    Engine::Math::Matrix4x4 viewProj;
};

struct InstanceData {
    Engine::Math::Matrix4x4 world;
    Engine::Math::Vector4 baseColor;
    uint32_t textureIndex;
    float _padding[3];
};

struct MaterialData {
    Engine::Math::Vector4 baseColor;
    uint32_t textureIndex;
    uint32_t _final_pad0[3];
};

} // namespace Engine::GeneratedSchema
