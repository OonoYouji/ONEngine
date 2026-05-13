#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Registry.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"
#include <cstdint>

namespace Engine::ECS {

///
/// スカイボックス描画情報を管理するシステム
///
class SkyboxSystem final : public System {
public:
    void Update(Registry& registry) override {
        // 最初に見つかったスカイボックスを使用
        registry.GetView<Skybox>().Each([&](Entity entity, Skybox& skybox) {
            if (found_) return;
            textureIndex_ = skybox.textureIndex;
            found_ = true;
        });
    }

    bool HasSkybox() const { return found_; }
    uint32_t GetTextureIndex() const { return textureIndex_; }
    
    void Reset() { found_ = false; }

private:
    bool found_ = false;
    uint32_t textureIndex_ = 0xFFFFFFFF;
};

} // namespace Engine::ECS
