#pragma once
#include "Engine/Core/Math/Math.h"
#include <cstdint>

namespace Engine::GeneratedSchema {

struct SceneData {
    Engine::Math::Matrix4x4 view;
    Engine::Math::Matrix4x4 viewProj;
    Engine::Math::Vector3 cameraPos;
    uint32_t numPointLights;
    float dirLightIntensity;
    Engine::Math::Vector3 dirLightColor;
    Engine::Math::Vector3 dirLightDirection;
    float screenWidth;
    float screenHeight;
    float nearZ;
    float farZ;
    uint8_t _final_pad0[4];
};

struct TextData {
    Engine::Math::Matrix4x4 world;
    Engine::Math::Vector4 color;
    Engine::Math::Vector2 uvMin;
    Engine::Math::Vector2 uvMax;
    uint32_t textureIndex;
    uint32_t padding;
    uint8_t _final_pad0[8];
};

struct InstanceData {
    Engine::Math::Matrix4x4 world;
    Engine::Math::Vector4 aabbMin;
    Engine::Math::Vector4 aabbMax;
    Engine::Math::Vector4 baseColor;
    uint32_t modelIndex;
    uint32_t textureIndex;
    uint32_t vertexOffset;
    uint32_t entityID;
    uint32_t postProcessFlags;
    uint32_t unused_pad[3];
};

struct PointLightData {
    Engine::Math::Vector3 position;
    float intensity;
    Engine::Math::Vector3 color;
    float radius;
};

struct ParticleGPUData {
    Engine::Math::Vector3 position;
    float age;
    Engine::Math::Vector3 velocity;
    float maxLifetime;
    Engine::Math::Vector4 color;
    Engine::Math::Vector3 rotation;
    float scale;
    uint32_t modelIndex;
    uint32_t textureIndex;
    uint32_t entityID;
    uint32_t postProcessFlags;
};

struct MeshInfo {
    uint32_t vertexOffset;
    uint32_t indexOffset;
    uint32_t vertexCount;
    uint32_t indexCount;
    uint32_t meshCount;
    uint8_t _final_pad0[12];
};

struct BoneData {
    Engine::Math::Matrix4x4 transform;
};

struct SpriteData {
    Engine::Math::Matrix4x4 world;
    Engine::Math::Vector4 color;
    uint32_t textureIndex;
    uint32_t isBillboard;
    uint32_t entityID;
    uint32_t postProcessFlags;
};

struct MaterialData {
    Engine::Math::Vector4 baseColor;
};

struct BoneWeightData {
    uint32_t boneIndices[4];
    Engine::Math::Vector4 boneWeights;
};

struct SkinningParams {
    uint32_t vertexCount;
    uint32_t inputVertexOffset;
    uint32_t outputVertexOffset;
    uint32_t boneOffset;
    uint32_t skinningEnabled;
    uint8_t _final_pad0[12];
};


} // namespace Engine::GeneratedSchema

namespace Engine::ECS {

struct TextRenderer {
    char text[256];
    uint32_t fontIndex;
    uint8_t _pad0[12];
    Engine::Math::Vector4 color;
    float size;
    uint32_t isScreenSpace;
    uint8_t _final_pad1[8];
};

struct SkinnedMeshRenderer {
    uint32_t modelIndex;
    uint32_t materialIndex;
    uint32_t skeletonIndex;
    uint32_t postProcessFlags;
    uint32_t internalVertexOffset;
    uint8_t _final_pad0[12];
};

struct Transform {
    uint32_t parent;
    Engine::Math::Vector3 position;
    Engine::Math::Vector3 rotation;
    uint8_t _pad0[4];
    Engine::Math::Vector3 scale;
    uint8_t _pad1[4];
    Engine::Math::Matrix4x4 world;
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

struct ParticleEmitter {
    uint32_t count;
    float speed;
    float speedRandom;
    float lifetime;
    float lifetimeRandom;
    float spreadAngle;
    float gravity;
    uint8_t _pad0[4];
    Engine::Math::Vector4 startColor;
    Engine::Math::Vector4 endColor;
    float startScale;
    float endScale;
    uint32_t modelIndex;
    uint32_t textureIndex;
    uint32_t bufferIndex;
    uint8_t _final_pad1[12];
};

struct DirectionalLight {
    Engine::Math::Vector3 color;
    float intensity;
    Engine::Math::Vector3 direction;
    uint8_t _final_pad0[4];
};

struct SpriteRenderer {
    uint32_t textureIndex;
    uint8_t _pad0[12];
    Engine::Math::Vector4 color;
    Engine::Math::Vector2 size;
    uint32_t isBillboard;
    uint8_t _final_pad1[4];
};

struct Tag {
    char name[256];
};

struct ScriptComponent {
    uint64_t gcHandle;
    uint32_t typeId;
    uint8_t _final_pad0[4];
};

struct MeshRenderer {
    uint32_t modelIndex;
    uint32_t materialIndex;
    uint32_t postProcessFlags;
    uint8_t _final_pad0[4];
};

struct Skybox {
    uint32_t textureIndex;
    uint8_t _final_pad0[12];
};

} // namespace Engine::ECS
