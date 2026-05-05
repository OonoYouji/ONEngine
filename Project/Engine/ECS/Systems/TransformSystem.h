#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Registry.h"
#include "Schema/Schema.h"
#include <unordered_set>
#include <vector>

namespace Engine::ECS {

///
/// 全てのTransformのワールド行列を親子関係を考慮して一括更新するシステム
///
class TransformSystem final : public System {
public:
    void Update(Registry& registry) override {
        // 更新済みフラグのクリア
        processed_.clear();
        
        // 全ての Transform を走査
        auto view = registry.GetView<Transform>();
        view.Each([&](Entity entity, Transform& transform) {
            UpdateRecursive(registry, entity, transform);
        });
    }

private:
    /// @brief 指定したエンティティとその親のワールド行列を再帰的に更新する (メモ化付き)
    /// @return 更新済みのワールド行列
    Engine::Math::Matrix4x4 UpdateRecursive(Registry& registry, Entity entity, Transform& t) {
        // すでにこのフレームで計算済みならキャッシュを返す
        if (processed_.count(entity)) {
            return t.world;
        }

        // 自身のローカル行列を計算
        auto local = Engine::Math::Matrix4x4::MakeAffine(t.scale, t.rotation, t.position);

        if (t.parent != kNullEntity && registry.HasComponent<Transform>(t.parent)) {
            // 親がいる場合: 親を先に更新（再帰）してから掛け合わせる
            auto& parentTransform = registry.GetComponent<Transform>(t.parent);
            t.world = local * UpdateRecursive(registry, t.parent, parentTransform);
        } else {
            // 親がいない（ルート）場合: ローカル行列がそのままワールド行列
            t.world = local;
        }

        // 計算済みとして記録
        processed_.insert(entity);
        return t.world;
    }

private:
    std::unordered_set<Entity> processed_;
};

} // namespace Engine::ECS
