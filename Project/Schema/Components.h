#pragma once
#include "Engine/Core/Math/Math.h"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <cstring>
#include <string>
#include <algorithm>

namespace Engine::ECS {

struct Camera {
    float fov = 0.0f;
    float nearZ = 0.0f;
    float farZ = 0.0f;
    uint8_t _final_pad0[4];
};

struct TextRenderer {
    char text[256];
    uint32_t fontIndex = 0;
    uint8_t _pad0[12];
    Engine::Math::Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    float size = 0.0f;
    uint32_t isScreenSpace = 0;
    uint8_t _final_pad1[8];
};

struct SkinnedMeshRenderer {
    uint32_t modelIndex = 0;
    uint32_t materialIndex = 0;
    uint32_t skeletonIndex = 0;
    uint32_t postProcessFlags = 0;
    uint32_t internalVertexOffset = 0;
    uint8_t _final_pad0[12];
};

struct Transform {
    uint32_t parent = 0;
    float sortOrder = 0.0f;
    uint8_t _pad0[8];
    Engine::Math::Vector3 position = { 0, 0, 0 };
    uint8_t _pad1[4];
    Engine::Math::Vector3 rotation = { 0, 0, 0 };
    uint8_t _pad2[4];
    Engine::Math::Vector3 scale = { 1.0f, 1.0f, 1.0f };
    uint8_t _pad3[4];
    Engine::Math::Matrix4x4 world = Engine::Math::Matrix4x4::kIdentity;
};

struct SpriteRenderer {
    uint32_t textureIndex = 0;
    uint8_t _pad0[12];
    Engine::Math::Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    Engine::Math::Vector2 size = { 0, 0 };
    uint32_t isBillboard = 0;
    uint8_t _final_pad1[4];
};

struct DirectionalLight {
    Engine::Math::Vector3 color = { 1.0f, 1.0f, 1.0f };
    float intensity = 0.0f;
    Engine::Math::Vector3 direction = { 0, 0, 0 };
    uint8_t _final_pad0[4];
};

struct Tag {
    char name[256];
};

struct PointLight {
    Engine::Math::Vector3 color = { 1.0f, 1.0f, 1.0f };
    float intensity = 0.0f;
    float radius = 0.0f;
    uint8_t _final_pad0[12];
};

struct ScriptComponent {
    uint64_t gcHandle = 0;
    uint32_t typeId = 0;
    uint8_t _final_pad0[4];
};

struct MeshRenderer {
    uint32_t modelIndex = 0;
    uint32_t materialIndex = 0;
    uint32_t postProcessFlags = 0;
    uint8_t _final_pad0[4];
};

struct ParticleEmitter {
    uint32_t count = 0;
    float speed = 0.0f;
    float speedRandom = 0.0f;
    float lifetime = 0.0f;
    float lifetimeRandom = 0.0f;
    float spreadAngle = 0.0f;
    float gravity = 0.0f;
    uint8_t _pad0[4];
    Engine::Math::Vector4 startColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    Engine::Math::Vector4 endColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    float startScale = 0.0f;
    float endScale = 0.0f;
    uint32_t modelIndex = 0;
    uint32_t textureIndex = 0;
    uint32_t bufferIndex = 0;
    uint8_t _final_pad1[12];
};

struct Skybox {
    uint32_t textureIndex = 0;
    uint8_t _final_pad0[12];
};

} // namespace Engine::ECS

inline void to_json(nlohmann::json& j, const Engine::ECS::Camera& v) {
    j = nlohmann::json{
        {"fov", v.fov},
        {"nearZ", v.nearZ},
        {"farZ", v.farZ},
    };
}

inline void from_json(const nlohmann::json& j, Engine::ECS::Camera& v) {
    if (j.contains("fov")) v.fov = j.at("fov").get<float>();
    if (j.contains("nearZ")) v.nearZ = j.at("nearZ").get<float>();
    if (j.contains("farZ")) v.farZ = j.at("farZ").get<float>();
}
inline void to_json(nlohmann::json& j, const Engine::ECS::TextRenderer& v) {
    j = nlohmann::json{
        {"text", std::string(v.text)},
        {"fontIndex", v.fontIndex},
        {"color", v.color},
        {"size", v.size},
        {"isScreenSpace", v.isScreenSpace},
    };
}

inline void from_json(const nlohmann::json& j, Engine::ECS::TextRenderer& v) {
    if (j.contains("text")) { std::string s = j.at("text").get<std::string>(); size_t len = (std::min)(s.length(), sizeof(v.text) - 1); std::memcpy(v.text, s.c_str(), len); v.text[len] = '\0'; }
    if (j.contains("fontIndex")) v.fontIndex = j.at("fontIndex").get<uint32_t>();
    if (j.contains("color")) v.color = j.at("color").get<Engine::Math::Vector4>();
    if (j.contains("size")) v.size = j.at("size").get<float>();
    if (j.contains("isScreenSpace")) v.isScreenSpace = j.at("isScreenSpace").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const Engine::ECS::SkinnedMeshRenderer& v) {
    j = nlohmann::json{
        {"modelIndex", v.modelIndex},
        {"materialIndex", v.materialIndex},
        {"skeletonIndex", v.skeletonIndex},
        {"postProcessFlags", v.postProcessFlags},
        {"internalVertexOffset", v.internalVertexOffset},
    };
}

inline void from_json(const nlohmann::json& j, Engine::ECS::SkinnedMeshRenderer& v) {
    if (j.contains("modelIndex")) v.modelIndex = j.at("modelIndex").get<uint32_t>();
    if (j.contains("materialIndex")) v.materialIndex = j.at("materialIndex").get<uint32_t>();
    if (j.contains("skeletonIndex")) v.skeletonIndex = j.at("skeletonIndex").get<uint32_t>();
    if (j.contains("postProcessFlags")) v.postProcessFlags = j.at("postProcessFlags").get<uint32_t>();
    if (j.contains("internalVertexOffset")) v.internalVertexOffset = j.at("internalVertexOffset").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const Engine::ECS::Transform& v) {
    j = nlohmann::json{
        {"parent", v.parent},
        {"sortOrder", v.sortOrder},
        {"position", v.position},
        {"rotation", v.rotation},
        {"scale", v.scale},
        {"world", v.world},
    };
}

inline void from_json(const nlohmann::json& j, Engine::ECS::Transform& v) {
    if (j.contains("parent")) v.parent = j.at("parent").get<uint32_t>();
    if (j.contains("sortOrder")) v.sortOrder = j.at("sortOrder").get<float>();
    if (j.contains("position")) v.position = j.at("position").get<Engine::Math::Vector3>();
    if (j.contains("rotation")) v.rotation = j.at("rotation").get<Engine::Math::Vector3>();
    if (j.contains("scale")) v.scale = j.at("scale").get<Engine::Math::Vector3>();
    if (j.contains("world")) v.world = j.at("world").get<Engine::Math::Matrix4x4>();
}
inline void to_json(nlohmann::json& j, const Engine::ECS::SpriteRenderer& v) {
    j = nlohmann::json{
        {"textureIndex", v.textureIndex},
        {"color", v.color},
        {"size", v.size},
        {"isBillboard", v.isBillboard},
    };
}

inline void from_json(const nlohmann::json& j, Engine::ECS::SpriteRenderer& v) {
    if (j.contains("textureIndex")) v.textureIndex = j.at("textureIndex").get<uint32_t>();
    if (j.contains("color")) v.color = j.at("color").get<Engine::Math::Vector4>();
    if (j.contains("size")) v.size = j.at("size").get<Engine::Math::Vector2>();
    if (j.contains("isBillboard")) v.isBillboard = j.at("isBillboard").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const Engine::ECS::DirectionalLight& v) {
    j = nlohmann::json{
        {"color", v.color},
        {"intensity", v.intensity},
        {"direction", v.direction},
    };
}

inline void from_json(const nlohmann::json& j, Engine::ECS::DirectionalLight& v) {
    if (j.contains("color")) v.color = j.at("color").get<Engine::Math::Vector3>();
    if (j.contains("intensity")) v.intensity = j.at("intensity").get<float>();
    if (j.contains("direction")) v.direction = j.at("direction").get<Engine::Math::Vector3>();
}
inline void to_json(nlohmann::json& j, const Engine::ECS::Tag& v) {
    j = nlohmann::json{
        {"name", std::string(v.name)},
    };
}

inline void from_json(const nlohmann::json& j, Engine::ECS::Tag& v) {
    if (j.contains("name")) { std::string s = j.at("name").get<std::string>(); size_t len = (std::min)(s.length(), sizeof(v.name) - 1); std::memcpy(v.name, s.c_str(), len); v.name[len] = '\0'; }
}
inline void to_json(nlohmann::json& j, const Engine::ECS::PointLight& v) {
    j = nlohmann::json{
        {"color", v.color},
        {"intensity", v.intensity},
        {"radius", v.radius},
    };
}

inline void from_json(const nlohmann::json& j, Engine::ECS::PointLight& v) {
    if (j.contains("color")) v.color = j.at("color").get<Engine::Math::Vector3>();
    if (j.contains("intensity")) v.intensity = j.at("intensity").get<float>();
    if (j.contains("radius")) v.radius = j.at("radius").get<float>();
}
inline void to_json(nlohmann::json& j, const Engine::ECS::ScriptComponent& v) {
    j = nlohmann::json{
        {"gcHandle", v.gcHandle},
        {"typeId", v.typeId},
    };
}

inline void from_json(const nlohmann::json& j, Engine::ECS::ScriptComponent& v) {
    if (j.contains("gcHandle")) v.gcHandle = j.at("gcHandle").get<uint64_t>();
    if (j.contains("typeId")) v.typeId = j.at("typeId").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const Engine::ECS::MeshRenderer& v) {
    j = nlohmann::json{
        {"modelIndex", v.modelIndex},
        {"materialIndex", v.materialIndex},
        {"postProcessFlags", v.postProcessFlags},
    };
}

inline void from_json(const nlohmann::json& j, Engine::ECS::MeshRenderer& v) {
    if (j.contains("modelIndex")) v.modelIndex = j.at("modelIndex").get<uint32_t>();
    if (j.contains("materialIndex")) v.materialIndex = j.at("materialIndex").get<uint32_t>();
    if (j.contains("postProcessFlags")) v.postProcessFlags = j.at("postProcessFlags").get<uint32_t>();
}
inline void to_json(nlohmann::json& j, const Engine::ECS::ParticleEmitter& v) {
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

inline void from_json(const nlohmann::json& j, Engine::ECS::ParticleEmitter& v) {
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
inline void to_json(nlohmann::json& j, const Engine::ECS::Skybox& v) {
    j = nlohmann::json{
        {"textureIndex", v.textureIndex},
    };
}

inline void from_json(const nlohmann::json& j, Engine::ECS::Skybox& v) {
    if (j.contains("textureIndex")) v.textureIndex = j.at("textureIndex").get<uint32_t>();
}

#ifdef ENGINE_EDITOR
#include "imgui.h"
#include "Editor/EditorUI.h"
namespace Engine::ECS {
template<typename TProp>
inline void DrawUI_Camera(Engine::ECS::Camera& v, TProp Prop) {
    Prop("Fov", [&]() { return ImGui::DragFloat("Fov", &v.fov, 0.1f); });
    Prop("NearZ", [&]() { return ImGui::DragFloat("NearZ", &v.nearZ, 0.1f); });
    Prop("FarZ", [&]() { return ImGui::DragFloat("FarZ", &v.farZ, 0.1f); });
}

template<typename TProp>
inline void DrawUI_TextRenderer(Engine::ECS::TextRenderer& v, TProp Prop) {
    Prop("Text", [&]() { return ImGui::InputText("Text", v.text, sizeof(v.text)); });
    Prop("FontIndex", [&]() { return Editor::EditorUI::AssetPicker("FontIndex", "Font", &v.fontIndex); });
    Prop("Color", [&]() { return ImGui::ColorEdit4("Color", &v.color.x); });
    Prop("Size", [&]() { return ImGui::DragFloat("Size", &v.size, 0.1f); });
    Prop("IsScreenSpace", [&]() { return ImGui::Checkbox("IsScreenSpace", (bool*)&v.isScreenSpace); });
}

template<typename TProp>
inline void DrawUI_SkinnedMeshRenderer(Engine::ECS::SkinnedMeshRenderer& v, TProp Prop) {
    Prop("ModelIndex", [&]() { return Editor::EditorUI::AssetPicker("ModelIndex", "Model", &v.modelIndex); });
    Prop("MaterialIndex", [&]() { return Editor::EditorUI::AssetPicker("MaterialIndex", "Material", &v.materialIndex); });
    Prop("SkeletonIndex", [&]() { return ImGui::InputScalar("SkeletonIndex", ImGuiDataType_U32, &v.skeletonIndex); });
    Prop("PostProcessFlags", [&]() { return ImGui::Checkbox("PostProcessFlags", (bool*)&v.postProcessFlags); });
    Prop("InternalVertexOffset", [&]() { return ImGui::InputScalar("InternalVertexOffset", ImGuiDataType_U32, &v.internalVertexOffset); });
}

template<typename TProp>
inline void DrawUI_Transform(Engine::ECS::Transform& v, TProp Prop) {
    Prop("Parent", [&]() { return ImGui::InputScalar("Parent", ImGuiDataType_U32, &v.parent); });
    Prop("SortOrder", [&]() { return ImGui::DragFloat("SortOrder", &v.sortOrder, 0.1f); });
    Prop("Position", [&]() { return ImGui::DragFloat3("Position", &v.position.x, 0.1f); });
    Prop("Rotation", [&]() { return ImGui::DragFloat3("Rotation", &v.rotation.x, 0.1f); });
    Prop("Scale", [&]() { return ImGui::DragFloat3("Scale", &v.scale.x, 0.1f); });
    ImGui::Text("World: Matrix4x4", "Matrix");
}

template<typename TProp>
inline void DrawUI_SpriteRenderer(Engine::ECS::SpriteRenderer& v, TProp Prop) {
    Prop("TextureIndex", [&]() { return Editor::EditorUI::AssetPicker("TextureIndex", "Texture", &v.textureIndex); });
    Prop("Color", [&]() { return ImGui::ColorEdit4("Color", &v.color.x); });
    Prop("Size", [&]() { return ImGui::DragFloat2("Size", &v.size.x, 0.1f); });
    Prop("IsBillboard", [&]() { return ImGui::Checkbox("IsBillboard", (bool*)&v.isBillboard); });
}

template<typename TProp>
inline void DrawUI_DirectionalLight(Engine::ECS::DirectionalLight& v, TProp Prop) {
    Prop("Color", [&]() { return ImGui::ColorEdit3("Color", &v.color.x); });
    Prop("Intensity", [&]() { return ImGui::DragFloat("Intensity", &v.intensity, 0.1f); });
    Prop("Direction", [&]() { return ImGui::DragFloat3("Direction", &v.direction.x, 0.1f); });
}

template<typename TProp>
inline void DrawUI_Tag(Engine::ECS::Tag& v, TProp Prop) {
    Prop("Name", [&]() { return ImGui::InputText("Name", v.name, sizeof(v.name)); });
}

template<typename TProp>
inline void DrawUI_PointLight(Engine::ECS::PointLight& v, TProp Prop) {
    Prop("Color", [&]() { return ImGui::ColorEdit3("Color", &v.color.x); });
    Prop("Intensity", [&]() { return ImGui::DragFloat("Intensity", &v.intensity, 0.1f); });
    Prop("Radius", [&]() { return ImGui::DragFloat("Radius", &v.radius, 0.1f); });
}

template<typename TProp>
inline void DrawUI_ScriptComponent(Engine::ECS::ScriptComponent& v, TProp Prop) {
    Prop("GcHandle", [&]() { return ImGui::InputScalar("GcHandle", ImGuiDataType_U64, &v.gcHandle); });
    Prop("TypeId", [&]() { return ImGui::InputScalar("TypeId", ImGuiDataType_U32, &v.typeId); });
}

template<typename TProp>
inline void DrawUI_MeshRenderer(Engine::ECS::MeshRenderer& v, TProp Prop) {
    Prop("ModelIndex", [&]() { return Editor::EditorUI::AssetPicker("ModelIndex", "Model", &v.modelIndex); });
    Prop("MaterialIndex", [&]() { return Editor::EditorUI::AssetPicker("MaterialIndex", "Material", &v.materialIndex); });
    Prop("PostProcessFlags", [&]() { return ImGui::Checkbox("PostProcessFlags", (bool*)&v.postProcessFlags); });
}

template<typename TProp>
inline void DrawUI_ParticleEmitter(Engine::ECS::ParticleEmitter& v, TProp Prop) {
    Prop("Count", [&]() { return ImGui::InputScalar("Count", ImGuiDataType_U32, &v.count); });
    Prop("Speed", [&]() { return ImGui::DragFloat("Speed", &v.speed, 0.1f); });
    Prop("SpeedRandom", [&]() { return ImGui::DragFloat("SpeedRandom", &v.speedRandom, 0.1f); });
    Prop("Lifetime", [&]() { return ImGui::DragFloat("Lifetime", &v.lifetime, 0.1f); });
    Prop("LifetimeRandom", [&]() { return ImGui::DragFloat("LifetimeRandom", &v.lifetimeRandom, 0.1f); });
    Prop("SpreadAngle", [&]() { return ImGui::DragFloat("SpreadAngle", &v.spreadAngle, 0.1f); });
    Prop("Gravity", [&]() { return ImGui::DragFloat("Gravity", &v.gravity, 0.1f); });
    Prop("StartColor", [&]() { return ImGui::ColorEdit4("StartColor", &v.startColor.x); });
    Prop("EndColor", [&]() { return ImGui::ColorEdit4("EndColor", &v.endColor.x); });
    Prop("StartScale", [&]() { return ImGui::DragFloat("StartScale", &v.startScale, 0.1f); });
    Prop("EndScale", [&]() { return ImGui::DragFloat("EndScale", &v.endScale, 0.1f); });
    Prop("ModelIndex", [&]() { return Editor::EditorUI::AssetPicker("ModelIndex", "Model", &v.modelIndex); });
    Prop("TextureIndex", [&]() { return Editor::EditorUI::AssetPicker("TextureIndex", "Texture", &v.textureIndex); });
    Prop("BufferIndex", [&]() { return ImGui::InputScalar("BufferIndex", ImGuiDataType_U32, &v.bufferIndex); });
}

template<typename TProp>
inline void DrawUI_Skybox(Engine::ECS::Skybox& v, TProp Prop) {
    Prop("TextureIndex", [&]() { return Editor::EditorUI::AssetPicker("TextureIndex", "Texture", &v.textureIndex); });
}

} // namespace Engine::ECS
#endif // ENGINE_EDITOR
