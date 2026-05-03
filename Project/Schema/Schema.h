#pragma once
#include "Engine/Core/Math/Math.h"
#include <cstdint>

namespace Engine::GeneratedSchema {

struct SceneData {
    Engine::Math::Matrix4x4 viewProj;
};

struct InstanceData {
    Engine::Math::Matrix4x4 world;
    uint32_t textureIndex;
};

struct MaterialData {
    uint32_t textureIndex;
    uint32_t _final_pad0[3];
};

} // namespace Engine::GeneratedSchema
