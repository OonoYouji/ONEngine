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

    // 全てのコンポーネントを、自動生成された to_json / from_json を利用して登録
    
    // Transform (ID: 1)
    reg.Register<Transform>(1, "Transform", 
        [](const json& j, Transform& t) { from_json(j, t); },
        [](const Transform& t) { json j; to_json(j, t); return j; }
    );

    // MeshRenderer (ID: 2)
    reg.Register<MeshRenderer>(2, "MeshRenderer",
        [](const json& j, MeshRenderer& m) { from_json(j, m); },
        [](const MeshRenderer& m) { json j; to_json(j, m); return j; }
    );

    // Camera (ID: 3)
    reg.Register<Camera>(3, "Camera",
        [](const json& j, Camera& c) { from_json(j, c); },
        [](const Camera& c) { json j; to_json(j, c); return j; }
    );

    // DirectionalLight (ID: 4)
    reg.Register<DirectionalLight>(4, "DirectionalLight",
        [](const json& j, DirectionalLight& l) { from_json(j, l); },
        [](const DirectionalLight& l) { json j; to_json(j, l); return j; }
    );

    // PointLight (ID: 5)
    reg.Register<PointLight>(5, "PointLight",
        [](const json& j, PointLight& l) { from_json(j, l); },
        [](const PointLight& l) { json j; to_json(j, l); return j; }
    );

    // SpriteRenderer (ID: 6)
    reg.Register<SpriteRenderer>(6, "SpriteRenderer",
        [](const json& j, SpriteRenderer& s) { from_json(j, s); },
        [](const SpriteRenderer& s) { json j; to_json(j, s); return j; }
    );

    // ParticleEmitter (ID: 7)
    reg.Register<ParticleEmitter>(7, "ParticleEmitter",
        [](const json& j, ParticleEmitter& p) { from_json(j, p); },
        [](const ParticleEmitter& p) { json j; to_json(j, p); return j; }
    );

    // Skybox (ID: 8)
    reg.Register<Skybox>(8, "Skybox",
        [](const json& j, Skybox& s) { from_json(j, s); },
        [](const Skybox& s) { json j; to_json(j, s); return j; }
    );

    // TextRenderer (ID: 9)
    reg.Register<TextRenderer>(9, "TextRenderer",
        [](const json& j, TextRenderer& tr) { from_json(j, tr); },
        [](const TextRenderer& tr) { json j; to_json(j, tr); return j; }
    );

    // Tag (ID: 100)
    reg.Register<Tag>(100, "Tag",
        [](const json& j, Tag& t) { from_json(j, t); },
        [](const Tag& t) { json j; to_json(j, t); return j; }
    );
}

} // namespace Engine::ECS
