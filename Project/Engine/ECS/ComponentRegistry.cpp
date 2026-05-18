#include "Engine/ECS/ComponentRegistry.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/MaterialManager.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Asset/FontManager.h"
#include "Engine/Script/ScriptHost.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"
#include <cstring>
#include <algorithm>

namespace Engine::ECS {

ComponentRegistry* ComponentRegistry::instance_ = nullptr;

void InitializeComponentRegistry() {
    auto& reg = ComponentRegistry::GetInstance();

    // ID 1: Transform
    reg.Register<Transform>(1, "Transform",
        [](const json& j, Transform& t) { from_json(j, t); },
        [](const Transform& t) { json j; to_json(j, t); return j; },
        [](void* data, PropertyFunc Prop) { DrawUI_Transform(*static_cast<Transform*>(data), Prop); }
    );

    // ID 2: MeshRenderer
    reg.Register<MeshRenderer>(2, "MeshRenderer",
        [](const json& j, MeshRenderer& m) {
            from_json(j, m);
            if (j.contains("modelGuid")) m.modelIndex = Asset::AssetManager::GetInstance().LoadModel(std::to_string(j["modelGuid"].get<uint64_t>()));
            if (j.contains("materialGuid")) m.materialIndex = Asset::MaterialManager::GetInstance().LoadMaterial(j["materialGuid"].get<uint64_t>());
        },
        [](const MeshRenderer& m) {
            json j; to_json(j, m);
            if (auto asset = Asset::AssetManager::GetInstance().GetModelByIndex(m.modelIndex)) j["modelGuid"] = asset->GetGuid();
            if (auto asset = Asset::MaterialManager::GetInstance().GetMaterialByIndex(m.materialIndex)) j["materialGuid"] = asset->GetGuid();
            return j;
        },
        [](void* data, PropertyFunc Prop) { DrawUI_MeshRenderer(*static_cast<MeshRenderer*>(data), Prop); }
    );

    // ID 3: ScriptComponent
    reg.Register<ScriptComponent>(3, "ScriptComponent",
        [](const json& j, ScriptComponent& s) { from_json(j, s); },
        [](const ScriptComponent& s, Entity entity, Registry& r) { 
            json j; to_json(j, s);
            auto& host = Engine::Script::ScriptHost::GetInstance();
            auto serializeFunc = (char*(*)(uint32_t))host.GetMethodDelegate(
                L"ONEngine.Scripting.EngineHost, ONEngine.Scripting",
                L"SerializeScriptToNative",
                L"");
            if (serializeFunc) {
                char* scriptJson = serializeFunc((uint32_t)entity);
                if (scriptJson) {
                    try {
                        j["scriptData"] = json::parse(scriptJson);
                    } catch (...) {}
                }
            }
            return j;
        },
        [](void* data, PropertyFunc Prop) { DrawUI_ScriptComponent(*static_cast<ScriptComponent*>(data), Prop); }
    );
    // ID 4: Camera
    reg.Register<Camera>(4, "Camera",
        [](const json& j, Camera& c) { from_json(j, c); },
        [](const Camera& c) { json j; to_json(j, c); return j; },
        [](void* data, PropertyFunc Prop) { DrawUI_Camera(*static_cast<Camera*>(data), Prop); }
    );

    // ID 5: DirectionalLight
    reg.Register<DirectionalLight>(5, "DirectionalLight",
        [](const json& j, DirectionalLight& l) { from_json(j, l); },
        [](const DirectionalLight& l) { json j; to_json(j, l); return j; },
        [](void* data, PropertyFunc Prop) { DrawUI_DirectionalLight(*static_cast<DirectionalLight*>(data), Prop); }
    );

    // ID 6: PointLight
    reg.Register<PointLight>(6, "PointLight",
        [](const json& j, PointLight& l) { from_json(j, l); },
        [](const PointLight& l) { json j; to_json(j, l); return j; },
        [](void* data, PropertyFunc Prop) { DrawUI_PointLight(*static_cast<PointLight*>(data), Prop); }
    );

    // ID 7: SpriteRenderer
    reg.Register<SpriteRenderer>(7, "SpriteRenderer",
        [](const json& j, SpriteRenderer& s) { 
            from_json(j, s);
            if (j.contains("textureGuid")) s.textureIndex = Asset::TextureManager::GetInstance().LoadTexture(j["textureGuid"].get<uint64_t>());
        },
        [](const SpriteRenderer& s) { 
            json j; to_json(j, s); 
            if (auto asset = Asset::TextureManager::GetInstance().GetTextureByIndex(s.textureIndex)) j["textureGuid"] = asset->GetGuid();
            return j; 
        },
        [](void* data, PropertyFunc Prop) { DrawUI_SpriteRenderer(*static_cast<SpriteRenderer*>(data), Prop); }
    );

    // ID 8: ParticleEmitter
    reg.Register<ParticleEmitter>(8, "ParticleEmitter",
        [](const json& j, ParticleEmitter& p) { 
            from_json(j, p);
            if (j.contains("textureGuid")) p.textureIndex = Asset::TextureManager::GetInstance().LoadTexture(j["textureGuid"].get<uint64_t>());
            if (j.contains("modelGuid")) p.modelIndex = Asset::AssetManager::GetInstance().LoadModel(std::to_string(j["modelGuid"].get<uint64_t>()));
        },
        [](const ParticleEmitter& p) { 
            json j; to_json(j, p); 
            if (auto asset = Asset::TextureManager::GetInstance().GetTextureByIndex(p.textureIndex)) j["textureGuid"] = asset->GetGuid();
            if (auto asset = Asset::AssetManager::GetInstance().GetModelByIndex(p.modelIndex)) j["modelGuid"] = asset->GetGuid();
            return j; 
        },
        [](void* data, PropertyFunc Prop) { DrawUI_ParticleEmitter(*static_cast<ParticleEmitter*>(data), Prop); }
    );

    // ID 9: Skybox
    reg.Register<Skybox>(9, "Skybox",
        [](const json& j, Skybox& s) { 
            from_json(j, s);
            if (j.contains("textureGuid")) s.textureIndex = Asset::TextureManager::GetInstance().LoadTexture(j["textureGuid"].get<uint64_t>());
        },
        [](const Skybox& s) { 
            json j; to_json(j, s); 
            if (auto asset = Asset::TextureManager::GetInstance().GetTextureByIndex(s.textureIndex)) j["textureGuid"] = asset->GetGuid();
            return j; 
        },
        [](void* data, PropertyFunc Prop) { DrawUI_Skybox(*static_cast<Skybox*>(data), Prop); }
    );

    // ID 10: TextRenderer
    reg.Register<TextRenderer>(10, "TextRenderer", 
        [](const json& j, TextRenderer& tr) { 
            from_json(j, tr);
            if (j.contains("fontGuid")) tr.fontIndex = Asset::FontManager::GetInstance().LoadFont(j["fontGuid"].get<uint64_t>());
        },
        [](const TextRenderer& tr) { 
            json j; to_json(j, tr); 
            if (auto asset = Asset::FontManager::GetInstance().GetFontByIndex(tr.fontIndex)) j["fontGuid"] = asset->GetGuid();
            return j; 
        },
        [](void* data, PropertyFunc Prop) { DrawUI_TextRenderer(*static_cast<TextRenderer*>(data), Prop); }
    );

    // ID 11: SkinnedMeshRenderer
    reg.Register<SkinnedMeshRenderer>(11, "SkinnedMeshRenderer",
        [](const json& j, SkinnedMeshRenderer& smr) {
            from_json(j, smr);
            if (j.contains("modelGuid")) smr.modelIndex = Asset::AssetManager::GetInstance().LoadModel(std::to_string(j["modelGuid"].get<uint64_t>()));
            if (j.contains("materialGuid")) smr.materialIndex = Asset::MaterialManager::GetInstance().LoadMaterial(j["materialGuid"].get<uint64_t>());
        },
        [](const SkinnedMeshRenderer& smr) { 
            json j; to_json(j, smr); 
            if (auto asset = Asset::AssetManager::GetInstance().GetModelByIndex(smr.modelIndex)) j["modelGuid"] = asset->GetGuid();
            if (auto asset = Asset::MaterialManager::GetInstance().GetMaterialByIndex(smr.materialIndex)) j["materialGuid"] = asset->GetGuid();
            return j; 
        },
        [](void* data, PropertyFunc Prop) { DrawUI_SkinnedMeshRenderer(*static_cast<SkinnedMeshRenderer*>(data), Prop); }
    );

    // ID 100: Tag
    reg.Register<Tag>(100, "Tag",
        [](const json& j, Tag& t) { from_json(j, t); },
        [](const Tag& t) { json j; to_json(j, t); return j; },
        [](void* data, PropertyFunc Prop) { DrawUI_Tag(*static_cast<Tag*>(data), Prop); }
    );}

} // namespace Engine::ECS
