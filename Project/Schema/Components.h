#pragma once
#include "Engine/Core/Math/Math.h"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <cstring>
#include <string>

namespace Engine::ECS {

struct Camera {
    float fov;
    float nearZ;
    float farZ;
    uint8_t _final_pad0[4];
};

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

struct SpriteRenderer {
    uint32_t textureIndex;
    uint8_t _pad0[12];
    Engine::Math::Vector4 color;
    Engine::Math::Vector2 size;
    uint32_t isBillboard;
    uint8_t _final_pad1[4];
};

struct DirectionalLight {
    Engine::Math::Vector3 color;
    float intensity;
    Engine::Math::Vector3 direction;
    uint8_t _final_pad0[4];
};

struct Tag {
    char name[256];
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
    uint32_t postProcessFlags;
    uint8_t _final_pad0[4];
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

struct Skybox {
    uint32_t textureIndex;
    uint8_t _final_pad0[12];
};

inline void to_json(nlohmann::json& j, const Camera& v) {
    j = nlohmann::json{
        {"fov", v.fov},
        {"nearZ", v.nearZ},
        {"farZ", v.farZ},
    };
}

inline void from_json(const nlohmann::json& j, Camera& v) {
    if (j.contains("fov")) v.fov = j.at("fov").get<float>();
    if (j.contains("nearZ")) v.nearZ = j.at("nearZ").get<float>();
    if (j.contains("farZ")) v.farZ = j.at("farZ").get<float>();
}
inline void to_json(nlohmann::json& j, const TextRenderer& v) {
    j = nlohmann::json{
        {"text", std::string(v.text)},
        {"fontIndex", v.fontIndex},
        {"color", v.color},
        {"size", v.size},
        {"isScreenSpace", v.isScreenSpace},
    };
}

inline void from_json(const nlohmann::json& j, TextRenderer& v) {
    if (j.contains("text")) { std::string s = j.at("text").get<std::string>(); strncpy(v.text, s.c_str(), sizeof(v.text) - 1); v.text[sizeof(v.text) - 1] = '\0'; }
    if (j.contains("fontIndex")) v.fontIndex = j.at("fontIndex").get<uint32_t>();
    if (j.contains("color")) v.color = j.at("color").get<Engine::Math::Vector4>();
    if (j.contains("size")) v.size = j.at("size").get<float>();
    if (j.contains("isScreenSpace")) v.isScreenSpace = j.at("isScreenSpace").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const SkinnedMeshRenderer& v) {
    j = nlohmann::json{
        {"modelIndex", v.modelIndex},
        {"materialIndex", v.materialIndex},
        {"skeletonIndex", v.skeletonIndex},
        {"postProcessFlags", v.postProcessFlags},
        {"internalVertexOffset", v.internalVertexOffset},
    };
}

inline void from_json(const nlohmann::json& j, SkinnedMeshRenderer& v) {
    if (j.contains("modelIndex")) v.modelIndex = j.at("modelIndex").get<uint32_t>();
    if (j.contains("materialIndex")) v.materialIndex = j.at("materialIndex").get<uint32_t>();
    if (j.contains("skeletonIndex")) v.skeletonIndex = j.at("skeletonIndex").get<uint32_t>();
    if (j.contains("postProcessFlags")) v.postProcessFlags = j.at("postProcessFlags").get<uint32_t>();
    if (j.contains("internalVertexOffset")) v.internalVertexOffset = j.at("internalVertexOffset").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const Transform& v) {
    j = nlohmann::json{
        {"parent", v.parent},
        {"position", v.position},
        {"rotation", v.rotation},
        {"scale", v.scale},
        {"world", v.world},
    };
}

inline void from_json(const nlohmann::json& j, Transform& v) {
    if (j.contains("parent")) v.parent = j.at("parent").get<uint32_t>();
    if (j.contains("position")) v.position = j.at("position").get<Engine::Math::Vector3>();
    if (j.contains("rotation")) v.rotation = j.at("rotation").get<Engine::Math::Vector3>();
    if (j.contains("scale")) v.scale = j.at("scale").get<Engine::Math::Vector3>();
    if (j.contains("world")) v.world = j.at("world").get<Engine::Math::Matrix4x4>();
}
inline void to_json(nlohmann::json& j, const SpriteRenderer& v) {
    j = nlohmann::json{
        {"textureIndex", v.textureIndex},
        {"color", v.color},
        {"size", v.size},
        {"isBillboard", v.isBillboard},
    };
}

inline void from_json(const nlohmann::json& j, SpriteRenderer& v) {
    if (j.contains("textureIndex")) v.textureIndex = j.at("textureIndex").get<uint32_t>();
    if (j.contains("color")) v.color = j.at("color").get<Engine::Math::Vector4>();
    if (j.contains("size")) v.size = j.at("size").get<Engine::Math::Vector2>();
    if (j.contains("isBillboard")) v.isBillboard = j.at("isBillboard").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const DirectionalLight& v) {
    j = nlohmann::json{
        {"color", v.color},
        {"intensity", v.intensity},
        {"direction", v.direction},
    };
}

inline void from_json(const nlohmann::json& j, DirectionalLight& v) {
    if (j.contains("color")) v.color = j.at("color").get<Engine::Math::Vector3>();
    if (j.contains("intensity")) v.intensity = j.at("intensity").get<float>();
    if (j.contains("direction")) v.direction = j.at("direction").get<Engine::Math::Vector3>();
}
inline void to_json(nlohmann::json& j, const Tag& v) {
    j = nlohmann::json{
        {"name", std::string(v.name)},
    };
}

inline void from_json(const nlohmann::json& j, Tag& v) {
    if (j.contains("name")) { std::string s = j.at("name").get<std::string>(); strncpy(v.name, s.c_str(), sizeof(v.name) - 1); v.name[sizeof(v.name) - 1] = '\0'; }
}
inline void to_json(nlohmann::json& j, const PointLight& v) {
    j = nlohmann::json{
        {"color", v.color},
        {"intensity", v.intensity},
        {"radius", v.radius},
    };
}

inline void from_json(const nlohmann::json& j, PointLight& v) {
    if (j.contains("color")) v.color = j.at("color").get<Engine::Math::Vector3>();
    if (j.contains("intensity")) v.intensity = j.at("intensity").get<float>();
    if (j.contains("radius")) v.radius = j.at("radius").get<float>();
}
inline void to_json(nlohmann::json& j, const ScriptComponent& v) {
    j = nlohmann::json{
        {"gcHandle", v.gcHandle},
        {"typeId", v.typeId},
    };
}

inline void from_json(const nlohmann::json& j, ScriptComponent& v) {
    if (j.contains("gcHandle")) v.gcHandle = j.at("gcHandle").get<uint64_t>();
    if (j.contains("typeId")) v.typeId = j.at("typeId").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const MeshRenderer& v) {
    j = nlohmann::json{
        {"modelIndex", v.modelIndex},
        {"materialIndex", v.materialIndex},
        {"postProcessFlags", v.postProcessFlags},
    };
}

inline void from_json(const nlohmann::json& j, MeshRenderer& v) {
    if (j.contains("modelIndex")) v.modelIndex = j.at("modelIndex").get<uint32_t>();
    if (j.contains("materialIndex")) v.materialIndex = j.at("materialIndex").get<uint32_t>();
    if (j.contains("postProcessFlags")) v.postProcessFlags = j.at("postProcessFlags").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const ParticleEmitter& v) {
    j = nlohmann::json{
        {"count", v.count},
        {"speed", v.speed},
        {"speedRandom", v.speedRandom},
        {"lifetime", v.lifetime},
        {"lifetimeRandom", v.lifetimeRandom},
        {"spreadAngle", v.spreadAngle},
        {"gravity", v.gravity},
        {"startColor", v.startColor},
        {"endColor", v.endColor},
        {"startScale", v.startScale},
        {"endScale", v.endScale},
        {"modelIndex", v.modelIndex},
        {"textureIndex", v.textureIndex},
        {"bufferIndex", v.bufferIndex},
    };
}

inline void from_json(const nlohmann::json& j, ParticleEmitter& v) {
    if (j.contains("count")) v.count = j.at("count").get<uint32_t>();
    if (j.contains("speed")) v.speed = j.at("speed").get<float>();
    if (j.contains("speedRandom")) v.speedRandom = j.at("speedRandom").get<float>();
    if (j.contains("lifetime")) v.lifetime = j.at("lifetime").get<float>();
    if (j.contains("lifetimeRandom")) v.lifetimeRandom = j.at("lifetimeRandom").get<float>();
    if (j.contains("spreadAngle")) v.spreadAngle = j.at("spreadAngle").get<float>();
    if (j.contains("gravity")) v.gravity = j.at("gravity").get<float>();
    if (j.contains("startColor")) v.startColor = j.at("startColor").get<Engine::Math::Vector4>();
    if (j.contains("endColor")) v.endColor = j.at("endColor").get<Engine::Math::Vector4>();
    if (j.contains("startScale")) v.startScale = j.at("startScale").get<float>();
    if (j.contains("endScale")) v.endScale = j.at("endScale").get<float>();
    if (j.contains("modelIndex")) v.modelIndex = j.at("modelIndex").get<uint32_t>();
    if (j.contains("textureIndex")) v.textureIndex = j.at("textureIndex").get<uint32_t>();
    if (j.contains("bufferIndex")) v.bufferIndex = j.at("bufferIndex").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const Skybox& v) {
    j = nlohmann::json{
        {"textureIndex", v.textureIndex},
    };
}

inline void from_json(const nlohmann::json& j, Skybox& v) {
    if (j.contains("textureIndex")) v.textureIndex = j.at("textureIndex").get<uint32_t>();
}
} // namespace Engine::ECS
