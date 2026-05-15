#pragma once
#include "Engine/ECS/Registry.h"
#include <vector>

namespace Engine::Editor {

namespace ECS = Engine::ECS;

class HierarchyView {
public:
    void Render(ECS::Registry& registry, bool* p_open = nullptr);

private:
    void DrawEntityNode(ECS::Registry& registry, ECS::Entity entity);
    void DrawDropSeparator(ECS::Registry& registry, uint32_t id, ECS::Entity parent, float newOrder, ECS::Entity blockedA, ECS::Entity blockedB);
    void UpdateEntityOrder(ECS::Registry& registry, ECS::Entity entity, ECS::Entity newParent, float newOrder);
    void ReparentEntity(ECS::Registry& registry, ECS::Entity entity, ECS::Entity newParent);
};

} // namespace Engine::Editor
