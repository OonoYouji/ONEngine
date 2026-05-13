#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Registry.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"
#include <vector>

namespace Engine::ECS {

///
/// 全てのTransformのワールド行列を親子関係を考慮して一括更新するシステム
///
class TransformSystem final : public System {
public:
    void Update(Registry& registry) override {
        uint32_t maxId = registry.GetMaxEntityId();
        
        // 必要なサイズを確保
        if (processed_.size() <= maxId) {
            processed_.resize(maxId + 1);
        }
        
        // フラグをリセット
        std::fill(processed_.begin(), processed_.end(), false);
        
        auto view = registry.GetView<Transform>();
        view.Each([&](Entity entity, Transform& transform) {
            UpdateRecursive(registry, entity, transform);
        });
    }

private:
    Engine::Math::Matrix4x4 UpdateRecursive(Registry& registry, Entity entity, Transform& t) {
        if (entity >= processed_.size()) return t.world; // 安全ガード
        if (processed_[entity]) return t.world;

        auto local = Engine::Math::Matrix4x4::MakeAffine(t.scale, t.rotation, t.position);

        if (t.parent != kNullEntity && registry.HasComponent<Transform>(t.parent)) {
            auto& parentTransform = registry.GetComponent<Transform>(t.parent);
            t.world = local * UpdateRecursive(registry, t.parent, parentTransform);
        } else {
            t.world = local;
        }

        processed_[entity] = true;
        return t.world;
    }

private:
    std::vector<bool> processed_;
};

} // namespace Engine::ECS
