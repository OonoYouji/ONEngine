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

void InitializeComponentRegistry() {
    auto& reg = ComponentRegistry::GetInstance();

    // ID 1: Transform
    reg.Register<Transform>(1, "Transform", 
        [](const json& j, Transform& t) { from_json(j, t); },
        [](const Transform& t) { json j; to_json(j, t); return j; }
    );

    // ID 2: MeshRenderer
    reg.Register<MeshRenderer>(2, "MeshRenderer",
        [](const json& j, MeshRenderer& m) { from_json(j, m); },
        [](const MeshRenderer& m) { json j; to_json(j, m); return j; }
    );

    // ID 3: ScriptComponent
    reg.Register<ScriptComponent>(3, "ScriptComponent",
        [](const json& j, ScriptComponent& s) { from_json(j, s); },
        [](const ScriptComponent& s) { json j; to_json(j, s); return j; }
    );

    // ID 4: Camera
    reg.Register<Camera>(4, "Camera",
        [](const json& j, Camera& c) { from_json(j, c); },
        [](const Camera& c) { json j; to_json(j, c); return j; }
    );

    // ID 5: DirectionalLight
    reg.Register<DirectionalLight>(5, "DirectionalLight",
        [](const json& j, DirectionalLight& l) { from_json(j, l); },
        [](const DirectionalLight& l) { json j; to_json(j, l); return j; }
    );

    // ID 6: PointLight
    reg.Register<PointLight>(6, "PointLight",
        [](const json& j, PointLight& l) { from_json(j, l); },
        [](const PointLight& l) { json j; to_json(j, l); return j; }
    );

    // ID 7: SpriteRenderer
    reg.Register<SpriteRenderer>(7, "SpriteRenderer",
        [](const json& j, SpriteRenderer& s) { from_json(j, s); },
        [](const SpriteRenderer& s) { json j; to_json(j, s); return j; }
    );

    // ID 8: ParticleEmitter
    reg.Register<ParticleEmitter>(8, "ParticleEmitter",
        [](const json& j, ParticleEmitter& p) { from_json(j, p); },
        [](const ParticleEmitter& p) { json j; to_json(j, p); return j; }
    );

    // ID 9: Skybox
    reg.Register<Skybox>(9, "Skybox",
        [](const json& j, Skybox& s) { from_json(j, s); },
        [](const Skybox& s) { json j; to_json(j, s); return j; }
    );

    // ID 10: TextRenderer
    reg.Register<TextRenderer>(10, "TextRenderer",
        [](const json& j, TextRenderer& tr) { from_json(j, tr); },
        [](const TextRenderer& tr) { json j; to_json(j, tr); return j; }
    );

    // ID 100: Tag
    reg.Register<Tag>(100, "Tag",
        [](const json& j, Tag& t) { from_json(j, t); },
        [](const Tag& t) { json j; to_json(j, t); return j; }
    );
}

} // namespace Engine::ECS
