#pragma once
#include "Engine/Core/Math/Math.h"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <cstring>
#include <string>

namespace Engine::GeneratedSchema {

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

struct SkinningParams {
    uint32_t vertexCount;
    uint32_t inputVertexOffset;
    uint32_t outputVertexOffset;
    uint32_t boneOffset;
    uint32_t skinningEnabled;
    uint8_t _final_pad0[12];
};

struct SpriteData {
    Engine::Math::Matrix4x4 world;
    Engine::Math::Vector4 color;
    uint32_t textureIndex;
    uint32_t isBillboard;
    uint32_t entityID;
    uint32_t postProcessFlags;
};

struct BoneWeightData {
    uint32_t boneIndices[4];
    Engine::Math::Vector4 boneWeights;
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

struct MaterialData {
    Engine::Math::Vector4 baseColor;
};

inline void to_json(nlohmann::json& j, const PointLightData& v) {
    j = nlohmann::json{
        {"position", v.position},
        {"intensity", v.intensity},
        {"color", v.color},
        {"radius", v.radius},
    };
}

inline void from_json(const nlohmann::json& j, PointLightData& v) {
    if (j.contains("position")) v.position = j.at("position").get<Engine::Math::Vector3>();
    if (j.contains("intensity")) v.intensity = j.at("intensity").get<float>();
    if (j.contains("color")) v.color = j.at("color").get<Engine::Math::Vector3>();
    if (j.contains("radius")) v.radius = j.at("radius").get<float>();
}
inline void to_json(nlohmann::json& j, const ParticleGPUData& v) {
    j = nlohmann::json{
        {"position", v.position},
        {"age", v.age},
        {"velocity", v.velocity},
        {"maxLifetime", v.maxLifetime},
        {"color", v.color},
        {"rotation", v.rotation},
        {"scale", v.scale},
        {"modelIndex", v.modelIndex},
        {"textureIndex", v.textureIndex},
        {"entityID", v.entityID},
        {"postProcessFlags", v.postProcessFlags},
    };
}

inline void from_json(const nlohmann::json& j, ParticleGPUData& v) {
    if (j.contains("position")) v.position = j.at("position").get<Engine::Math::Vector3>();
    if (j.contains("age")) v.age = j.at("age").get<float>();
    if (j.contains("velocity")) v.velocity = j.at("velocity").get<Engine::Math::Vector3>();
    if (j.contains("maxLifetime")) v.maxLifetime = j.at("maxLifetime").get<float>();
    if (j.contains("color")) v.color = j.at("color").get<Engine::Math::Vector4>();
    if (j.contains("rotation")) v.rotation = j.at("rotation").get<Engine::Math::Vector3>();
    if (j.contains("scale")) v.scale = j.at("scale").get<float>();
    if (j.contains("modelIndex")) v.modelIndex = j.at("modelIndex").get<uint32_t>();
    if (j.contains("textureIndex")) v.textureIndex = j.at("textureIndex").get<uint32_t>();
    if (j.contains("entityID")) v.entityID = j.at("entityID").get<uint32_t>();
    if (j.contains("postProcessFlags")) v.postProcessFlags = j.at("postProcessFlags").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const SceneData& v) {
    j = nlohmann::json{
        {"view", v.view},
        {"viewProj", v.viewProj},
        {"cameraPos", v.cameraPos},
        {"numPointLights", v.numPointLights},
        {"dirLightIntensity", v.dirLightIntensity},
        {"dirLightColor", v.dirLightColor},
        {"dirLightDirection", v.dirLightDirection},
        {"screenWidth", v.screenWidth},
        {"screenHeight", v.screenHeight},
        {"nearZ", v.nearZ},
        {"farZ", v.farZ},
    };
}

inline void from_json(const nlohmann::json& j, SceneData& v) {
    if (j.contains("view")) v.view = j.at("view").get<Engine::Math::Matrix4x4>();
    if (j.contains("viewProj")) v.viewProj = j.at("viewProj").get<Engine::Math::Matrix4x4>();
    if (j.contains("cameraPos")) v.cameraPos = j.at("cameraPos").get<Engine::Math::Vector3>();
    if (j.contains("numPointLights")) v.numPointLights = j.at("numPointLights").get<uint32_t>();
    if (j.contains("dirLightIntensity")) v.dirLightIntensity = j.at("dirLightIntensity").get<float>();
    if (j.contains("dirLightColor")) v.dirLightColor = j.at("dirLightColor").get<Engine::Math::Vector3>();
    if (j.contains("dirLightDirection")) v.dirLightDirection = j.at("dirLightDirection").get<Engine::Math::Vector3>();
    if (j.contains("screenWidth")) v.screenWidth = j.at("screenWidth").get<float>();
    if (j.contains("screenHeight")) v.screenHeight = j.at("screenHeight").get<float>();
    if (j.contains("nearZ")) v.nearZ = j.at("nearZ").get<float>();
    if (j.contains("farZ")) v.farZ = j.at("farZ").get<float>();
}
inline void to_json(nlohmann::json& j, const TextData& v) {
    j = nlohmann::json{
        {"world", v.world},
        {"color", v.color},
        {"uvMin", v.uvMin},
        {"uvMax", v.uvMax},
        {"textureIndex", v.textureIndex},
        {"padding", v.padding},
    };
}

inline void from_json(const nlohmann::json& j, TextData& v) {
    if (j.contains("world")) v.world = j.at("world").get<Engine::Math::Matrix4x4>();
    if (j.contains("color")) v.color = j.at("color").get<Engine::Math::Vector4>();
    if (j.contains("uvMin")) v.uvMin = j.at("uvMin").get<Engine::Math::Vector2>();
    if (j.contains("uvMax")) v.uvMax = j.at("uvMax").get<Engine::Math::Vector2>();
    if (j.contains("textureIndex")) v.textureIndex = j.at("textureIndex").get<uint32_t>();
    if (j.contains("padding")) v.padding = j.at("padding").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const MeshInfo& v) {
    j = nlohmann::json{
        {"vertexOffset", v.vertexOffset},
        {"indexOffset", v.indexOffset},
        {"vertexCount", v.vertexCount},
        {"indexCount", v.indexCount},
        {"meshCount", v.meshCount},
    };
}

inline void from_json(const nlohmann::json& j, MeshInfo& v) {
    if (j.contains("vertexOffset")) v.vertexOffset = j.at("vertexOffset").get<uint32_t>();
    if (j.contains("indexOffset")) v.indexOffset = j.at("indexOffset").get<uint32_t>();
    if (j.contains("vertexCount")) v.vertexCount = j.at("vertexCount").get<uint32_t>();
    if (j.contains("indexCount")) v.indexCount = j.at("indexCount").get<uint32_t>();
    if (j.contains("meshCount")) v.meshCount = j.at("meshCount").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const BoneData& v) {
    j = nlohmann::json{
        {"transform", v.transform},
    };
}

inline void from_json(const nlohmann::json& j, BoneData& v) {
    if (j.contains("transform")) v.transform = j.at("transform").get<Engine::Math::Matrix4x4>();
}
inline void to_json(nlohmann::json& j, const SkinningParams& v) {
    j = nlohmann::json{
        {"vertexCount", v.vertexCount},
        {"inputVertexOffset", v.inputVertexOffset},
        {"outputVertexOffset", v.outputVertexOffset},
        {"boneOffset", v.boneOffset},
        {"skinningEnabled", v.skinningEnabled},
    };
}

inline void from_json(const nlohmann::json& j, SkinningParams& v) {
    if (j.contains("vertexCount")) v.vertexCount = j.at("vertexCount").get<uint32_t>();
    if (j.contains("inputVertexOffset")) v.inputVertexOffset = j.at("inputVertexOffset").get<uint32_t>();
    if (j.contains("outputVertexOffset")) v.outputVertexOffset = j.at("outputVertexOffset").get<uint32_t>();
    if (j.contains("boneOffset")) v.boneOffset = j.at("boneOffset").get<uint32_t>();
    if (j.contains("skinningEnabled")) v.skinningEnabled = j.at("skinningEnabled").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const SpriteData& v) {
    j = nlohmann::json{
        {"world", v.world},
        {"color", v.color},
        {"textureIndex", v.textureIndex},
        {"isBillboard", v.isBillboard},
        {"entityID", v.entityID},
        {"postProcessFlags", v.postProcessFlags},
    };
}

inline void from_json(const nlohmann::json& j, SpriteData& v) {
    if (j.contains("world")) v.world = j.at("world").get<Engine::Math::Matrix4x4>();
    if (j.contains("color")) v.color = j.at("color").get<Engine::Math::Vector4>();
    if (j.contains("textureIndex")) v.textureIndex = j.at("textureIndex").get<uint32_t>();
    if (j.contains("isBillboard")) v.isBillboard = j.at("isBillboard").get<uint32_t>();
    if (j.contains("entityID")) v.entityID = j.at("entityID").get<uint32_t>();
    if (j.contains("postProcessFlags")) v.postProcessFlags = j.at("postProcessFlags").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const BoneWeightData& v) {
    j = nlohmann::json{
        {"boneIndices", v.boneIndices},
        {"boneWeights", v.boneWeights},
    };
}

inline void from_json(const nlohmann::json& j, BoneWeightData& v) {
    if (j.contains("boneIndices")) { auto& arr = j.at("boneIndices"); for(int i=0; i<4; ++i) v.boneIndices[i] = arr.at(i).get<uint32_t>(); }
    if (j.contains("boneWeights")) v.boneWeights = j.at("boneWeights").get<Engine::Math::Vector4>();
}
inline void to_json(nlohmann::json& j, const InstanceData& v) {
    j = nlohmann::json{
        {"world", v.world},
        {"aabbMin", v.aabbMin},
        {"aabbMax", v.aabbMax},
        {"baseColor", v.baseColor},
        {"modelIndex", v.modelIndex},
        {"textureIndex", v.textureIndex},
        {"vertexOffset", v.vertexOffset},
        {"entityID", v.entityID},
        {"postProcessFlags", v.postProcessFlags},
        {"unused_pad", v.unused_pad},
    };
}

inline void from_json(const nlohmann::json& j, InstanceData& v) {
    if (j.contains("world")) v.world = j.at("world").get<Engine::Math::Matrix4x4>();
    if (j.contains("aabbMin")) v.aabbMin = j.at("aabbMin").get<Engine::Math::Vector4>();
    if (j.contains("aabbMax")) v.aabbMax = j.at("aabbMax").get<Engine::Math::Vector4>();
    if (j.contains("baseColor")) v.baseColor = j.at("baseColor").get<Engine::Math::Vector4>();
    if (j.contains("modelIndex")) v.modelIndex = j.at("modelIndex").get<uint32_t>();
    if (j.contains("textureIndex")) v.textureIndex = j.at("textureIndex").get<uint32_t>();
    if (j.contains("vertexOffset")) v.vertexOffset = j.at("vertexOffset").get<uint32_t>();
    if (j.contains("entityID")) v.entityID = j.at("entityID").get<uint32_t>();
    if (j.contains("postProcessFlags")) v.postProcessFlags = j.at("postProcessFlags").get<uint32_t>();
    if (j.contains("unused_pad")) { auto& arr = j.at("unused_pad"); for(int i=0; i<3; ++i) v.unused_pad[i] = arr.at(i).get<uint32_t>(); }
}
inline void to_json(nlohmann::json& j, const MaterialData& v) {
    j = nlohmann::json{
        {"baseColor", v.baseColor},
    };
}

inline void from_json(const nlohmann::json& j, MaterialData& v) {
    if (j.contains("baseColor")) v.baseColor = j.at("baseColor").get<Engine::Math::Vector4>();
}
} // namespace Engine::GeneratedSchema
