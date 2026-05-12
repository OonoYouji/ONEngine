#pragma once
#include "Engine/ECS/Registry.h"

namespace Engine::Editor {

namespace ECS = Engine::ECS;

class InspectorView {
public:
    void Render(ECS::Registry& registry);
};

} // namespace Engine::Editor
