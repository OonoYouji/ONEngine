#pragma once
#include "Engine/ECS/Registry.h"
#include "Schema/Components.h"

namespace Engine::Scene {

class HierarchySystem {
public:
    /// @brief 指定したエンティティが別のエンティティの子孫であるかを確認する
    static bool IsDescendantOf(ECS::Registry& registry, ECS::Entity entity, ECS::Entity potentialAncestor) {
        if (entity == ECS::kNullEntity || potentialAncestor == ECS::kNullEntity) return false;
        if (entity == potentialAncestor) return true;
        
        ECS::Entity current = entity;
        while (current != ECS::kNullEntity) {
            if (registry.HasComponent<ECS::Transform>(current)) {
                current = registry.GetComponent<ECS::Transform>(current).parent;
                if (current == potentialAncestor) return true;
            } else {
                break;
            }
        }
        return false;
    }

    /// @brief 親子関係を設定する（エディター外用、Undo/Redoなし）
    static void SetParent(ECS::Registry& registry, ECS::Entity entity, ECS::Entity newParent) {
        if (entity == ECS::kNullEntity || entity == newParent) return;
        
        // 循環参照のチェック（親を自分の子にしようとしていないか）
        if (newParent != ECS::kNullEntity && IsDescendantOf(registry, newParent, entity)) {
            return;
        }

        if (registry.HasComponent<ECS::Transform>(entity)) {
            registry.GetComponent<ECS::Transform>(entity).parent = newParent;
        }
    }
};

} // namespace Engine::Scene
