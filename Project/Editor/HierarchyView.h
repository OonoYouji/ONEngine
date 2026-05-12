#pragma once
#include "Engine/ECS/Registry.h"

namespace Engine::Editor {

namespace ECS = Engine::ECS;

class HierarchyView {
public:
    void Render(ECS::Registry& registry);

private:
    void DrawEntityNode(ECS::Registry& registry, ECS::Entity entity);
};

} // namespace Engine::Editor
