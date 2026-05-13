#include "Engine/ECS/ComponentRegistry.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/MaterialManager.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Asset/FontManager.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"
#include <cstring>

namespace Engine::ECS {

ComponentRegistry* ComponentRegistry::instance_ = nullptr;

namespace {
    // JSON から Vector3 を安全に取得するヘルパー
    Math::Vector3 GetVector3(const json& j, const std::string& key, const Math::Vector3& def = {0,0,0}) {
        if (!j.contains(key)) return def;
        const auto& v = j[key];
        return { v.value("x", def.x), v.value("y", def.y), v.value("z", def.z) };
    }

    // JSON から Vector4 を安全に取得するヘルパー
    Math::Vector4 GetVector4(const json& j, const std::string& key, const Math::Vector4& def = {0,0,0,0}) {
        if (!j.contains(key)) return def;
        const auto& v = j[key];
        return { v.value("x", def.x), v.value("y", def.y), v.value("z", def.z), v.value("w", def.w) };
    }

    // Vector3 を JSON に変換するヘルパー
    json Vector3ToJSON(const Math::Vector3& v) {
        return json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
    }

    // Vector4 を JSON に変換するヘルパー
    json Vector4ToJSON(const Math::Vector4& v) {
        return json{ {"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w} };
    }
}

void InitializeComponentRegistry() {
    auto& reg = ComponentRegistry::GetInstance();

    // Transform
    reg.Register<Transform>(1, "Transform", 
        [](const json& j, Transform& t) { 
            t.position = GetVector3(j, "position", {0,0,0});
            t.rotation = GetVector3(j, "rotation", {0,0,0});
            t.scale    = GetVector3(j, "scale", {1,1,1});
            t.parent   = j.value("parent", (Entity)0);
        },
        [](const Transform& t) { 
            json j; 
            j["position"] = Vector3ToJSON(t.position);
            j["rotation"] = Vector3ToJSON(t.rotation);
            j["scale"]    = Vector3ToJSON(t.scale);
            j["parent"]   = t.parent;
            return j; 
        }
    );

    // MeshRenderer
    reg.Register<MeshRenderer>(2, "MeshRenderer",
        [](const json& j, MeshRenderer& m) { 
            m.modelIndex = j.value("modelIndex", (uint32_t)0);
            m.materialIndex = j.value("materialIndex", (uint32_t)0);
            m.postProcessFlags = j.value("postProcessFlags", (uint32_t)0);
        },
        [](const MeshRenderer& m) { 
            json j; 
            j["modelIndex"] = m.modelIndex;
            j["materialIndex"] = m.materialIndex;
            j["postProcessFlags"] = m.postProcessFlags;
            return j; 
        }
    );

    // Camera
    reg.Register<Camera>(3, "Camera",
        [](const json& j, Camera& c) { 
            c.fov = j.value("fov", 45.0f);
            c.nearZ = j.value("nearZ", 0.1f);
            c.farZ = j.value("farZ", 1000.0f);
        },
        [](const Camera& c) { 
            json j; 
            j["fov"] = c.fov;
            j["nearZ"] = c.nearZ;
            j["farZ"] = c.farZ;
            return j; 
        }
    );

    // DirectionalLight
    reg.Register<DirectionalLight>(4, "DirectionalLight",
        [](const json& j, DirectionalLight& l) { 
            l.color = GetVector3(j, "color", {1,1,1});
            l.intensity = j.value("intensity", 1.0f);
            l.direction = GetVector3(j, "direction", {0,-1,0});
        },
        [](const DirectionalLight& l) { 
            json j; 
            j["color"] = Vector3ToJSON(l.color);
            j["intensity"] = l.intensity;
            j["direction"] = Vector3ToJSON(l.direction);
            return j; 
        }
    );

    // PointLight
    reg.Register<PointLight>(5, "PointLight",
        [](const json& j, PointLight& l) { 
            l.color = GetVector3(j, "color", {1,1,1});
            l.intensity = j.value("intensity", 1.0f);
            l.radius = j.value("radius", 10.0f);
        },
        [](const PointLight& l) { 
            json j; 
            j["color"] = Vector3ToJSON(l.color);
            j["intensity"] = l.intensity;
            j["radius"] = l.radius;
            return j; 
        }
    );

    // SpriteRenderer
    reg.Register<SpriteRenderer>(6, "SpriteRenderer",
        [](const json& j, SpriteRenderer& s) {
            if (j.contains("texturePath")) s.textureIndex = Asset::TextureManager::GetInstance().LoadTexture(j["texturePath"]);
            else s.textureIndex = j.value("textureIndex", (uint32_t)0);
            s.color = GetVector4(j, "color", {1,1,1,1});
            s.size = { j.value("size_x", 1.0f), j.value("size_y", 1.0f) };
            s.isBillboard = j.value("isBillboard", (uint32_t)0);
        },
        [](const SpriteRenderer& s) { 
            json j; 
            j["textureIndex"] = s.textureIndex;
            j["color"] = Vector4ToJSON(s.color);
            j["size_x"] = s.size.x;
            j["size_y"] = s.size.y;
            j["isBillboard"] = s.isBillboard;
            return j; 
        }
    );

    // ParticleEmitter
    reg.Register<ParticleEmitter>(7, "ParticleEmitter",
        [](const json& j, ParticleEmitter& p) { 
            // 簡易実装
        },
        [](const ParticleEmitter& p) { 
            return json::object(); 
        }
    );

    // Skybox
    reg.Register<Skybox>(8, "Skybox",
        [](const json& j, Skybox& s) {
            if (j.contains("texturePath")) s.textureIndex = Asset::TextureManager::GetInstance().LoadTexture(j["texturePath"]);
            else s.textureIndex = j.value("textureIndex", (uint32_t)0);
        },
        [](const Skybox& s) { 
            json j; 
            j["textureIndex"] = s.textureIndex;
            return j; 
        }
    );

    // Tag
    reg.Register<Tag>(100, "Tag",
        [](const json& j, Tag& t) {
            std::string name = j.value("name", "New Entity");
            memset(t.name, 0, sizeof(t.name));
            strncpy_s(t.name, name.c_str(), _TRUNCATE);
        },
        [](const Tag& t) { json j; j["name"] = std::string(t.name); return j; }
    );

    // TextRenderer
    reg.Register<TextRenderer>(9, "TextRenderer",
        [](const json& j, TextRenderer& tr) {
            std::string text = j.value("text", "");
            memset(tr.text, 0, sizeof(tr.text));
            strncpy_s(tr.text, text.c_str(), _TRUNCATE);
            tr.fontIndex = j.value("fontIndex", (uint32_t)0);
            tr.color = GetVector4(j, "color", {1,1,1,1});
            tr.size = j.value("size", 1.0f);
            tr.isScreenSpace = j.value("isScreenSpace", (uint32_t)0);
        },
        [](const TextRenderer& tr) {
            json j;
            j["text"] = std::string(tr.text);
            j["fontIndex"] = tr.fontIndex;
            j["color"] = Vector4ToJSON(tr.color);
            j["size"] = tr.size;
            j["isScreenSpace"] = tr.isScreenSpace;
            return j;
        }
    );
}

} // namespace Engine::ECS
