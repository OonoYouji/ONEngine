#pragma once
#include "Engine/Core/Math/Math.h"
#include <cstdint>

namespace Engine::GeneratedSchema {

struct PointLightData {
    Engine::Math::Vector3 position;
    float intensity;
    Engine::Math::Vector3 color;
    float radius;
};

struct SceneData {
    Engine::Math::Matrix4x4 viewProj;
    Engine::Math::Vector3 cameraPos;
    uint32_t numPointLights;
    float dirLightIntensity;
    Engine::Math::Vector3 dirLightColor;
    Engine::Math::Vector3 dirLightDirection;
    float padding;
};

struct InstanceData {
    Engine::Math::Matrix4x4 world;
    Engine::Math::Vector4 baseColor;
    uint32_t textureIndex;
    uint32_t vertexOffset;
    Engine::Math::Vector2 padding;
};

struct MaterialData {
    Engine::Math::Vector4 baseColor;
};


} // namespace Engine::GeneratedSchema

namespace Engine::ECS {

struct Transform {
    Engine::Math::Vector3 position;
    uint8_t _pad0[4];
    Engine::Math::Vector3 rotation;
    uint8_t _pad1[4];
    Engine::Math::Vector3 scale;
    uint8_t _final_pad2[4];
};

struct Camera {
    float fov;
    float nearZ;
    float farZ;
    uint8_t _final_pad0[4];
};

struct PointLight {
    Engine::Math::Vector3 color;
    float intensity;
    float radius;
    uint8_t _final_pad0[12];
};

struct ScriptComponent {
    uint64_t gcHandle;
    uint32_t typeId;
    uint8_t _final_pad0[4];
};

struct MeshRenderer {
    uint32_t modelIndex;
    uint32_t materialIndex;
    uint8_t _final_pad0[8];
};

struct DirectionalLight {
    Engine::Math::Vector3 color;
    float intensity;
    Engine::Math::Vector3 direction;
    uint8_t _final_pad0[4];
};

} // namespace Engine::ECS
