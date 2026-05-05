#include "Engine/ECS/ComponentRegistry.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/MaterialManager.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Asset/FontManager.h"
#include "Schema/Schema.h"

namespace Engine::ECS {

void InitializeComponentRegistry() {
    auto& reg = ComponentRegistry::GetInstance();

    // Transform
    reg.Register<Transform>(1, "Transform", [](const json& j, Transform& t) {
        t.parent = j.value("parent", 0); // 0 (kNullEntity) by default
        
        if (j.contains("position")) {
            t.position.x = j["position"].value("x", 0.0f);
            t.position.y = j["position"].value("y", 0.0f);
            t.position.z = j["position"].value("z", 0.0f);
        }
        if (j.contains("rotation")) {
            t.rotation.x = j["rotation"].value("x", 0.0f);
            t.rotation.y = j["rotation"].value("y", 0.0f);
            t.rotation.z = j["rotation"].value("z", 0.0f);
        }
        if (j.contains("scale")) {
            t.scale.x = j["scale"].value("x", 1.0f);
            t.scale.y = j["scale"].value("y", 1.0f);
            t.scale.z = j["scale"].value("z", 1.0f);
        }
    });

    // MeshRenderer
    reg.Register<MeshRenderer>(2, "MeshRenderer", [](const json& j, MeshRenderer& mr) {
        if (j.contains("meshPath")) mr.modelIndex = Asset::AssetManager::GetInstance().LoadModel(j["meshPath"]);
        else mr.modelIndex = j.value("modelIndex", 0);

        if (j.contains("materialPath")) mr.materialIndex = Asset::MaterialManager::GetInstance().LoadMaterial(j["materialPath"]);
        else mr.materialIndex = j.value("materialIndex", 0);
    });

    // Camera
    reg.Register<Camera>(4, "Camera", [](const json& j, Camera& c) {
        c.fov = j.value("fov", 45.0f);
        c.nearZ = j.value("nearZ", 0.1f);
        c.farZ = j.value("farZ", 1000.0f);
    });

    // DirectionalLight
    reg.Register<DirectionalLight>(5, "DirectionalLight", [](const json& j, DirectionalLight& l) {
        if (j.contains("color")) {
            l.color.x = j["color"].value("x", 1.0f);
            l.color.y = j["color"].value("y", 1.0f);
            l.color.z = j["color"].value("z", 1.0f);
        }
        l.intensity = j.value("intensity", 1.0f);
        if (j.contains("direction")) {
            l.direction.x = j["direction"].value("x", 0.0f);
            l.direction.y = j["direction"].value("y", -1.0f);
            l.direction.z = j["direction"].value("z", 1.0f);
        }
    });

    // PointLight
    reg.Register<PointLight>(6, "PointLight", [](const json& j, PointLight& l) {
        if (j.contains("color")) {
            l.color.x = j["color"].value("x", 1.0f);
            l.color.y = j["color"].value("y", 1.0f);
            l.color.z = j["color"].value("z", 1.0f);
        }
        l.intensity = j.value("intensity", 1.0f);
        l.radius = j.value("radius", 10.0f);
    });

    // SpriteRenderer
    reg.Register<SpriteRenderer>(7, "SpriteRenderer", [](const json& j, SpriteRenderer& sr) {
        if (j.contains("texturePath")) sr.textureIndex = Asset::TextureManager::GetInstance().LoadTexture(j["texturePath"]);
        else sr.textureIndex = j.value("textureIndex", 0);

        if (j.contains("color")) {
            sr.color.x = j["color"].value("x", 1.0f);
            sr.color.y = j["color"].value("y", 1.0f);
            sr.color.z = j["color"].value("z", 1.0f);
            sr.color.w = j["color"].value("w", 1.0f);
        }
        if (j.contains("size")) {
            sr.size.x = j["size"].value("x", 1.0f);
            sr.size.y = j["size"].value("y", 1.0f);
        }
        sr.isBillboard = j.value("isBillboard", 1) ? 1 : 0;
    });

    // Skybox
    reg.Register<Skybox>(8, "Skybox", [](const json& j, Skybox& s) {
        if (j.contains("texturePath")) s.textureIndex = Asset::TextureManager::GetInstance().LoadTexture(j["texturePath"]);
        else s.textureIndex = j.value("textureIndex", 0);
    });

    // TextRenderer
    reg.Register<TextRenderer>(9, "TextRenderer", [](const json& j, TextRenderer& tr) {
        std::string text = j.value("text", "");
        memset(tr.text, 0, sizeof(tr.text));
        strncpy_s(tr.text, text.c_str(), _TRUNCATE);

        if (j.contains("fontPath")) tr.fontIndex = Asset::FontManager::GetInstance().LoadFont(j["fontPath"]);
        else tr.fontIndex = j.value("fontIndex", 0);

        if (j.contains("color")) {
            tr.color.x = j["color"].value("x", 1.0f);
            tr.color.y = j["color"].value("y", 1.0f);
            tr.color.z = j["color"].value("z", 1.0f);
            tr.color.w = j["color"].value("w", 1.0f);
        }
        tr.size = j.value("size", 1.0f);
        tr.isScreenSpace = j.value("isScreenSpace", 0) ? 1 : 0;
    });

    // Script (特別扱い: AddScriptByName を呼ぶ必要があるため、Registry 自体はここでは ScriptComponent を持たないかもしれないが、
    // 将来的には ScriptComponent も他のコンポーネントと同じように扱えるようにする。)
}

} // namespace Engine::ECS
